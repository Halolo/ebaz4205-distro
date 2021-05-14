#include "led.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#define REGS  (3)

typedef struct {
  int fd;
  uint32_t map_size;
  void *map_base;
  volatile uint32_t *regs[REGS];
} led_hdl_t;

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

led_color_rgb_t hsv_to_rgb(led_color_hsv_t hsv) {
  led_color_rgb_t rgb;
  unsigned char region, remainder, p, q, t;

  if (hsv.s == 0) {
    rgb.r = hsv.v;
    rgb.g = hsv.v;
    rgb.b = hsv.v;
    return rgb;
  }

  region = hsv.h / 43;
  remainder = (hsv.h - (region * 43)) * 6;

  p = (hsv.v * (255 - hsv.s)) >> 8;
  q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
  t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
    case 0:
      rgb.r = hsv.v; rgb.g = t; rgb.b = p;
      break;
    case 1:
      rgb.r = q; rgb.g = hsv.v; rgb.b = p;
      break;
    case 2:
      rgb.r = p; rgb.g = hsv.v; rgb.b = t;
      break;
    case 3:
      rgb.r = p; rgb.g = q; rgb.b = hsv.v;
      break;
    case 4:
      rgb.r = t; rgb.g = p; rgb.b = hsv.v;
      break;
    default:
      rgb.r = hsv.v; rgb.g = p; rgb.b = q;
      break;
  }

  return rgb;
}

void *led_init(E_LED_CTRL ctrl) {
  led_hdl_t *pled_handler = malloc(sizeof(led_hdl_t));
  uint32_t led_ip_addr = 0;

  switch(ctrl) {
    case E_LED_CTRL_A4:
      led_ip_addr = 0x43C50000;
      break;
    case E_LED_CTRL_A3:
      led_ip_addr = 0x43C40000;
      break;
    case E_LED_CTRL_A2:
      led_ip_addr = 0x43C30000;
      break;
    case E_LED_CTRL_A1:
      led_ip_addr = 0x43C20000;
      break;
    case E_LED_CTRL_A0:
    default:
      led_ip_addr = 0x43C10000;
      break;
  }

  if (pled_handler) {
    pled_handler->fd = -1;
    pled_handler->map_size = 3 * sizeof(uint32_t);
    pled_handler->map_base = NULL;
    if ((pled_handler->fd = open("/dev/mem", O_RDWR | O_SYNC)) > 0) {
      printf("/dev/mem opened.\n");
      fflush(stdout);

      pled_handler->map_base = mmap(0, pled_handler->map_size, PROT_READ | PROT_WRITE, MAP_SHARED, pled_handler->fd, led_ip_addr);
      if(pled_handler->map_base) {
        printf("Memory mapped at address %p.\n", pled_handler->map_base);
        fflush(stdout);

        for (int r = 0; r < REGS; r++) {
          pled_handler->regs[r] = &((uint32_t *)(pled_handler->map_base))[r];
        }
      } else {
        fprintf(stderr, "Error (%d) [%s]\n", errno, strerror(errno));
        close(pled_handler->fd);
      }
    } else {
      fprintf(stderr, "Error (%d) [%s]\n", errno, strerror(errno));
    }
  }

  return pled_handler;
}

void led_close(void *hdl) {
  if (hdl) {
    led_hdl_t *pled_handler = (led_hdl_t *)hdl;

    if (pled_handler->map_base) {
      if(munmap(pled_handler->map_base, pled_handler->map_size) == -1) {
        fprintf(stderr, "Error (%d) [%s]\n", errno, strerror(errno));
      }
    }

    if (pled_handler->fd > 0) {
      close(pled_handler->fd);
    }

    free(pled_handler);
  }
}

inline void wait_reg(uint32_t *reg, uint32_t val) {
  int i = 0;

  while (val != *reg) {
    if (i == 10) {
      fprintf(stderr, "Timeout...\n");
      break;
    } else {
      usleep(100);
      i++;
    }
  }
}

void led_set_color_rgb(void *hdl, uint32_t idx, led_color_rgb_t color) {
  if (hdl) {
    led_hdl_t *pled_handler = (led_hdl_t *)hdl;

    uint32_t value = (reverse(color.g) & 0x000000FF) |
    (reverse(color.r) & 0x000000FF) << 8 |
    (reverse(color.b) & 0x000000FF) << 16;

    *pled_handler->regs[1] = idx;
    *pled_handler->regs[2] = value;
    *pled_handler->regs[0] = 1;
    wait_reg(pled_handler->regs[0], 0);
  }
}

void led_set_color_hsv(void *hdl, uint32_t idx, led_color_hsv_t color) {
  led_color_rgb_t rgb = hsv_to_rgb(color);
  led_set_color_rgb(hdl, idx, rgb);
}

void led_display(void *hdl) {
  if (hdl) {
    led_hdl_t *pled_handler = (led_hdl_t *)hdl;

    *pled_handler->regs[0] = 2;
    wait_reg(pled_handler->regs[0], 0);
  }
}

void led_reset(void *hdl) {
  if (hdl) {
    led_hdl_t *pled_handler = (led_hdl_t *)hdl;
    volatile uint32_t *reg = &((uint32_t *)(pled_handler->map_base))[0];

    *pled_handler->regs[0] = 4;
    wait_reg(pled_handler->regs[0], 0);
  }
}
