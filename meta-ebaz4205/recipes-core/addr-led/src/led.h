#ifndef LED_H
#define LED_H

#include <stdint.h>

typedef enum {
  E_LED_CTRL_A0 = 0,
  E_LED_CTRL_A1,
  E_LED_CTRL_A2,
  E_LED_CTRL_A3,
  E_LED_CTRL_A4,
  E_LED_CTRL_NUM
} E_LED_CTRL;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_color_rgb_t;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} led_color_hsv_t;

static const led_color_rgb_t LED_COLOR_RED     = { 020, 000, 000 };
static const led_color_rgb_t LED_COLOR_GREEN   = { 000, 020, 000 };
static const led_color_rgb_t LED_COLOR_BLUE    = { 000, 000, 020 };
static const led_color_rgb_t LED_COLOR_YELLOW  = { 020, 020, 000 };
static const led_color_rgb_t LED_COLOR_CYAN    = { 000, 020, 020 };
static const led_color_rgb_t LED_COLOR_MAGENTA = { 020, 000, 020 };
static const led_color_rgb_t LED_COLOR_WHITE   = { 020, 020, 020 };
static const led_color_rgb_t LED_COLOR_BLACK   = { 000, 000, 000 };

void *led_init(E_LED_CTRL ctrl);
void led_close(void *hdl);

void led_set_color_rgb(void *hdl, uint32_t idx, led_color_rgb_t color);
void led_set_color_hsv(void *hdl, uint32_t idx, led_color_hsv_t color);

void led_display(void *hdl);
void led_reset(void *hdl);

#endif // LED_H
