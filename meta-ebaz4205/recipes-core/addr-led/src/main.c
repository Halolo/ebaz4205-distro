#include "led.h"

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>

bool run = true;

void handle_sigint(int sig) {
  printf("Caught signal %d\n", sig);

  run = false;
}

int main(int argc, char **argv) {
  int brightness = 50;
  int controler = E_LED_CTRL_A0;
  uint8_t leds = 7;
  int index;
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "b:c:n:")) != -1) {
    switch (c) {
      case 'b':
        brightness = atoi(optarg);
        break;
      case 'c':
        controler = atoi(optarg);
        break;
      case 'n':
        leds = atoi(optarg);
        break;
      case '?':
        if ((optopt == 'b') || (optopt == 'c') || (optopt == 'n')) {
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        } else if (isprint (optopt)) {
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        } else {
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        return 1;
      default:
        return 1;
    }
  }

  printf ("brightness = %d, controler = %d, leds = %d\n",
    brightness, controler, leds);

  void *hdl = led_init(controler);
  led_color_hsv_t hsv[leds];

  signal(SIGINT, handle_sigint);
  signal(SIGTERM, handle_sigint);
  signal(SIGQUIT, handle_sigint);

  for (int i = 0; i < leds; i++) {
    hsv[i].v = brightness;
    hsv[i].s = 255;
    hsv[i].h = i * (255 / leds);
  }

  led_reset(hdl);
  led_display(hdl);

  while (run) {
    for (int i = 0; i < leds; i++) {
      led_set_color_hsv(hdl, i, hsv[i]);
      hsv[i].h++;
    }
    led_display(hdl);
  }

  led_reset(hdl);
  led_display(hdl);

  led_close(hdl);

  return 0;
}
