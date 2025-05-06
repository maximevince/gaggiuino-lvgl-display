#pragma once

#ifdef NATIVE_BUILD
#include <SDL2/SDL.h>
#include "lvgl.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* texture;

void display_init() {
    // SDL initialization is handled in lvgl_init()
}

void display_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p) {
    int32_t w = (area->x2 - area->x1 + 1);
    int32_t h = (area->y2 - area->y1 + 1);

    SDL_UpdateTexture(texture, NULL, color_p, w * sizeof(lv_color_t));
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    lv_disp_flush_ready(disp_drv);
}

void touchscreen_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data) {
    SDL_Event event;
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                last_x = event.motion.x;
                last_y = event.motion.y;
                data->point.x = last_x;
                data->point.y = last_y;
                data->state = LV_INDEV_STATE_PRESSED;
                break;
            case SDL_MOUSEBUTTONDOWN:
                last_x = event.button.x;
                last_y = event.button.y;
                data->point.x = last_x;
                data->point.y = last_y;
                data->state = LV_INDEV_STATE_PRESSED;
                break;
            case SDL_MOUSEBUTTONUP:
                data->point.x = last_x;
                data->point.y = last_y;
                data->state = LV_INDEV_STATE_RELEASED;
                break;
            default:
                data->state = LV_INDEV_STATE_RELEASED;
                break;
        }
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
#endif 