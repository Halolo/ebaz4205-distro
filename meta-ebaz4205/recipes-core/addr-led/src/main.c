#ifdef __STDC_NO_ATOMICS__
  #error this implementation needs atomics
#endif

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdatomic.h>
#include <string.h>

#include "MQTTClient.h"

#include "led.h"

#define S_RGB           (3)
#define FLOWERS         (16)
#define LED_PER_FLOWER  (4)

typedef struct {
  uint8_t *data;
  uint8_t colors;
} palette_t;

typedef enum {
  E_RAINBOW = 0,
  E_PALETTES,
  E_MODE_NUM
} E_MODES;

typedef enum {
  E_ON = 0,
  E_OFF,
  E_REFRESH,
  E_IDLE,
  E_EVENT_NUM
} E_EVENTS;

typedef struct {
  uint8_t mode:4;
  uint8_t event:4;
  uint8_t index;
} shared_t;

const uint8_t beach[][S_RGB] = {{150,206,180},{255,238,173},{255,111,105},{255,204,92},{136,216,176}};
const uint8_t purple_1[][S_RGB] = {{239,187,255},{216,150,255},{190,41,236},{128,0,128},{102,0,102}};
const uint8_t purple_2[][S_RGB] = {{108,54,132},{143,74,151},{136,120,178},{183,130,184},{222,202,227}};
const uint8_t neon[][S_RGB] = {{77,238,234},{116,238,21},{255,231,0},{240,0,255},{0,30,255}};
const uint8_t red[][S_RGB] = {{255,186,186},{255,123,123},{255,82,82},{255,0,0},{167,0,0}};
const uint8_t eighties[][S_RGB] = {{255,72,196},{43,209,252},{243,234,95},{192,77,249},{255,63,63}};
const uint8_t twilight[][S_RGB] = {{54,59,116},{103,56,136},{239,79,145},{199,157,215},{77,27,123}};
const uint8_t accent[][S_RGB] = {{0xb5, 0x89, 0x00}, {0xcb, 0x4b, 0x16}, {0xdc, 0x32, 0x2f}, {0xd3, 0x36, 0x82}, {0x6c, 0x71, 0xc4}, {0x26, 0x8b, 0xd2}, {0x2a, 0xa1, 0x98}, {0x85, 0x99, 0x00}};
const uint8_t circle[][S_RGB] = {{0xe0, 0x45, 0x6d}, {0xdf, 0x70, 0x45}, {0xdf, 0xbf, 0x46}, {0xac, 0xdf, 0x46}, {0x30, 0xe4, 0x13}, {0x45, 0xde, 0x83}, {0x45, 0xdf, 0xd3}, {0x42, 0xa5, 0xff}, {0x4c, 0x4b, 0xf1}, {0xa2, 0x45, 0xde}};

const palette_t palettes[] = {
  {.data = (uint8_t *)beach, .colors = sizeof(beach) / S_RGB},
  {.data = (uint8_t *)purple_1, .colors = sizeof(purple_1) / S_RGB},
  {.data = (uint8_t *)purple_2, .colors = sizeof(purple_2) / S_RGB},
  {.data = (uint8_t *)neon, .colors = sizeof(neon) / S_RGB},
  {.data = (uint8_t *)red, .colors = sizeof(red) / S_RGB},
  {.data = (uint8_t *)eighties, .colors = sizeof(eighties) / S_RGB},
  {.data = (uint8_t *)twilight, .colors = sizeof(twilight) / S_RGB},
  {.data = (uint8_t *)accent, .colors = sizeof(accent) / S_RGB},
  {.data = (uint8_t *)circle, .colors = sizeof(circle) / S_RGB},
};

bool run = true;

_Atomic shared_t status = {.mode = E_RAINBOW, .event = E_IDLE, .index = 0};

void handle_sigint(int sig) {
  printf("Caught signal %d\n", sig);

  run = false;
}

int msgarrvd(void *context, char *topic_name, int topic_len, MQTTClient_message *message) {
  static const uint8_t nb_palettes = sizeof(palettes) / sizeof(palette_t);
  shared_t tmp = status;

  if (0 == strcmp(topic_name, "/led-strip/mode")) {
    tmp.event = E_REFRESH;
    tmp.mode = (tmp.mode + 1) % E_MODE_NUM;
  } else if (0 == strcmp(topic_name, "/led-strip/on")) {
    tmp.event = E_ON;
  } else if (0 == strcmp(topic_name, "/led-strip/off")) {
    tmp.event = E_OFF;
  } else if (0 == strcmp(topic_name, "/led-strip/palettes/cycle")) {
    tmp.event = E_REFRESH;
    tmp.index = (tmp.index + 1) % nb_palettes;
  } else if (0 == strcmp(topic_name, "/led-strip/palettes/update")) {
    tmp.event = E_REFRESH;
  } else {
    fprintf(stderr, "Invalid topic: %s\n", topic_name);
  }

  atomic_store_explicit(&status, tmp, memory_order_relaxed);

  MQTTClient_freeMessage(&message);
  MQTTClient_free(topic_name);

  return 1;
}
void connlost(void *context, char *cause) {
  fprintf(stderr, "Connection lost (%s)\n", cause);
}

void mode_palettes(void *hdl, bool *on, uint8_t event, uint8_t index) {
  static led_color_rgb_t storage[FLOWERS];

  switch (event) {
    case E_OFF:
      if (*on) {
        *on = false;
        led_reset(hdl);
        led_display(hdl);
      }
    break;

    case E_ON:
      *on = true;

    case E_REFRESH: {
      uint8_t (*palette)[][S_RGB] = (uint8_t (*)[][S_RGB])palettes[index].data;

      for (int f = 0; f < FLOWERS; f++) {
        uint8_t color = rand() % palettes[index].colors;
        storage[f].r = (*palette)[color][1];
        storage[f].g = (*palette)[color][0];
        storage[f].b = (*palette)[color][2];
        for (int l = 0; l < LED_PER_FLOWER; l++) {
          led_set_color_rgb(hdl, (f * LED_PER_FLOWER) + l, storage[f]);
        }
      }
    }
    break;

    case E_IDLE:
      if (*on) {
        led_display(hdl);
      } else {
        usleep(10000);
      }
    break;

    default:
      fprintf(stderr, "Invalid event (%d)\n", event);
    break;
  }
}

void mode_rainbow(void *hdl, bool *on, uint8_t event) {
  static led_color_hsv_t storage[FLOWERS * LED_PER_FLOWER];

  switch (event) {
    case E_OFF:
      if (*on) {
        *on = false;
        led_reset(hdl);
        led_display(hdl);
      }
    break;

    case E_ON:
      *on = true;

    case E_REFRESH:
      for (int f = 0; f < (FLOWERS * LED_PER_FLOWER); f++) {
        storage[f].v = 128;
        storage[f].s = 255;
        storage[f].h = f * (255 / (FLOWERS * LED_PER_FLOWER));
      }
    break;

    case E_IDLE:
      if (*on) {
        for (int f = 0; f < (FLOWERS * LED_PER_FLOWER); f++) {
          led_set_color_hsv(hdl, f, storage[f]);
          storage[f].h++;
        }
        led_display(hdl);
      } else {
        usleep(10000);
      }
    break;

    default:
      fprintf(stderr, "Invalid event (%d)\n", event);
    break;
  }
}

int main(int argc, char **argv) {
  E_LED_CTRL controller = E_LED_CTRL_A1;
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  void *hdl = led_init(controller);
  bool on = false;

  int rc = 0;

  srand(time(NULL));

  signal(SIGINT, handle_sigint);
  signal(SIGTERM, handle_sigint);
  signal(SIGQUIT, handle_sigint);

  MQTTClient_create(&client, "tcp://localhost:1883", "led-strip-ctrl",
    MQTTCLIENT_PERSISTENCE_NONE, NULL);

  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, NULL);

  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
    fprintf(stderr, "Failed to connect, return code %d\n", rc);
  } else {
    MQTTClient_subscribe(client, "/led-strip/#", 2);
  }

  while (run) {
    shared_t tmp = status;

    switch (tmp.mode) {
      case E_RAINBOW:
        mode_rainbow(hdl, &on, tmp.event);
      break;

      case E_PALETTES:
        mode_palettes(hdl, &on, tmp.event, tmp.index);
      break;

      default:
        fprintf(stderr, "Invalid mode (%d)\n", tmp.mode);
        usleep(10000);
      break;
    }

    if (tmp.event != E_IDLE) {
      tmp.event = E_IDLE;
      atomic_store_explicit(&status, tmp, memory_order_relaxed);
    }
  }

  led_reset(hdl);
  led_display(hdl);

  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);

  led_close(hdl);

  return 0;
}
