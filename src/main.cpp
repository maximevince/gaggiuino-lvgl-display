#ifdef NATIVE
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#else
#include <Arduino.h>
#endif

#include "display.hpp"
#include "lv_demo_widgets.h"


/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

#ifdef USE_SDL
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
#endif

void lvgl_init()
{
  lv_init();

#ifdef NATIVE
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return;
  }

  // Create window
  window = SDL_CreateWindow("LVGL SDL Demo",
                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                          800, 480, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return;
  }

  // Create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    return;
  }

  // Create texture
  texture = SDL_CreateTexture(renderer,
                            SDL_PIXELFORMAT_RGB565,
                            SDL_TEXTUREACCESS_STREAMING,
                            800, 480);
  if (!texture) {
    printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    return;
  }

  screenWidth = 800;
  screenHeight = 480;
#else
  screenWidth = gfx->width();
  screenHeight = gfx->height();
#endif

#ifdef ESP32
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 200, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#else
  disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * 200);
#endif

  if (!disp_draw_buf)
  {
    Serial.println("LVGL disp_draw_buf allocate failed!");
    return;
  }

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 200);

  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);

  /* Setup display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = display_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the touchscreen input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchscreen_read;
  lv_indev_drv_register(&indev_drv);
}

void setup()
{
#ifdef NATIVE
  // SDL initialization is handled in lvgl_init()
#else
  Serial.begin(115200);
  delay(3000);
  Serial.println("LVGL Widgets Demo");
  display_init();
#endif

  lvgl_init();

  /* Draw demo widgets */
  lv_demo_widgets();

#ifdef NATIVE
  printf("Setup done\n");
#else
  Serial.println("Setup done");
#endif
}

void loop()
{
#ifdef NATIVE
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      SDL_Quit();
      exit(0);
    }
  }
#endif

  lv_timer_handler(); /* LVGL task */
  delay(5);
}
