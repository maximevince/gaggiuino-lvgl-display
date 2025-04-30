#include <unistd.h>
#define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lvgl.h"
#include "lv_demo_widgets.h"
#include "lv_drivers/sdl/sdl.h"
#include "lv_gaggiuino_ui.h"
#include <argparse/argparse.hpp>
#include "serial.h"
#include "nextion_parser.h"

#if LV_USE_LOG != 0
static void lv_log_print_g_cb(const char * buf)
{
    printf("%s", buf);
}
#endif

void lv_app_init(void)
{
    static lv_color_t buf1[480 * 10];
    static lv_color_t buf2[480 * 10];
    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, 480 * 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = sdl_display_flush;
    disp_drv.hor_res = 480;
    disp_drv.ver_res = 480;
    lv_disp_drv_register(&disp_drv);

    // Handle mouse input
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = sdl_mouse_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv);
}

int main(int argc, char* argv[])
{
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

    sdl_init();

    /* create Widgets on the screen */
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
