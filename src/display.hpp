#pragma once

#include <Arduino_GFX_Library.h>
#include <lvgl.h>

extern Arduino_RGB_Display *gfx;

void display_init();
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void touchscreen_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
