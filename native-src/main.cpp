#include <unistd.h>

#define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "drivers/sdl/lv_sdl_mouse.h"
#include "drivers/sdl/lv_sdl_mousewheel.h"
#include "drivers/sdl/lv_sdl_keyboard.h"

#include "lvgl.h"
#include "lv_gaggiuino_ui.h"
#include "serial.h"
#include "nextion_parser.h"

// Argparse does not work on OSX
#if 0
#include <argparse/argparse.hpp>
#else
#include <string>
#endif

#define SDL_HOR_RES 480
#define SDL_VER_RES 480

static lv_display_t *lvDisplay;
static lv_indev_t *lvMouse;
static lv_indev_t *lvMouseWheel;
static lv_indev_t *lvKeyboard;


#if LV_USE_LOG != 0
static void lv_log_print_g_cb(const char * buf)
{
    printf("%s", buf);
}
#endif

void lv_app_init(void)
{
    // static lv_color_t buf1[480 * 10];
    // static lv_color_t buf2[480 * 10];

    // lv_display_t * disp = lv_display_create(480, 480);
    // lv_display_set_flush_cb(disp, sdl_display_flush);
    // lv_display_set_draw_buffers(disp, buf1, buf2); //, sizeof(buf1)); //, LV_IMG_CF_TRUE_COLOR);

    // lv_indev_t * indev = lv_indev_create();
    // lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    // lv_indev_set_read_cb(indev, sdl_mouse_read);

    lvDisplay = lv_sdl_window_create(SDL_HOR_RES, SDL_VER_RES);
    lvMouse = lv_sdl_mouse_create();
    lvMouseWheel = lv_sdl_mousewheel_create();
    lvKeyboard = lv_sdl_keyboard_create();
}

int main(int argc, char* argv[])
{
#if 0
    argparse::ArgumentParser program("gaggiuino-lvgl-display");

    program.add_argument("--port")
        .help("Serial port to listen to (e.g. /dev/ttyUSB0)")
        .default_value(std::string(""));

    program.add_argument("--baudrate")
        .help("Serial port baudrate")
        .default_value(115200)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    std::string port = program.get<std::string>("--port");
    int baudrate = program.get<int>("--baudrate");
#else
    std::string port = "";
    int baudrate = 115200;
#endif

    if (!port.empty()) {
        printf("Listening on port %s at %d baud\n", port.c_str(), baudrate);
        if (serial_init(port.c_str(), baudrate, nextion_msg_handler_process) != 0) {
            fprintf(stderr, "Failed to initialize serial port\n");
            return 1;
        }
    }

    /* initialize lvgl */
    lv_init();

    /* Register the log print callback */
    #if LV_USE_LOG != 0
    lv_log_register_print_cb(lv_log_print_g_cb);
    #endif

    // sdl_init();

    /* create LVGL display and input devices */
    lv_app_init();

    /* Draw demo widgets */
    lv_gaggiuino_ui_init();

    Uint32 lastTick = SDL_GetTicks();
    while(1) {
        Uint32 current = SDL_GetTicks();
        lv_tick_inc(current - lastTick);
        lastTick = current;

        lv_timer_handler(); /* LVGL task */
        serial_task();      /* Serial task */

        SDL_Delay(5);
    }

    serial_close();
    return 0;
}
