/**
 * @file lv_gaggiuino_ui.c
 * Gaggiuino UI implementation
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>

#include "lv_gaggiuino_ui.h"
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define NAV_BAR_HEIGHT 40
#define SPLASH_DISPLAY_TIME 500 //2000  // 2 seconds

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    PAGE_SPLASH,
    PAGE_HOME,
    PAGE_BREW,
    PAGE_PLOT,
    PAGE_CLEAN,
    PAGE_SETTINGS,
    PAGE_COUNT
} page_id_t;

typedef enum {
    OBJ_POPUP_MSG,
    OBJ_PROFILE_1,
    OBJ_PROFILE_2,
    OBJ_PROFILE_3,
    OBJ_PROFILE_4,
    OBJ_PROFILE_5,
    OBJ_COUNT
} obj_id_t;

typedef struct {
    const char* name;
    lv_obj_t* obj;
} obj_lut_entry_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_splash_screen(lv_obj_t * parent);
static void create_home_screen(lv_obj_t * parent);
static void create_brew_screen(lv_obj_t * parent);
static void create_plot_screen(lv_obj_t * parent);
static void create_clean_screen(lv_obj_t * parent);
static void create_settings_screen(lv_obj_t * parent);
static void create_nav_bar(lv_obj_t * parent);
static void nav_event_cb(lv_event_t * e);
static void close_modal_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * tv;  // Tab view for main navigation
static lv_obj_t * splash_screen;
static lv_timer_t * splash_timer;
static lv_obj_t * popup_window;  // Modal window for messages
static lv_obj_t * temp_gauge;

// Object lookup table
static obj_lut_entry_t obj_lut[] = {
    [OBJ_POPUP_MSG] = {"popupMSG.t0", NULL},  // Will be initialized when popup is created
    [OBJ_PROFILE_1] = {"home.qPf1", NULL},
    [OBJ_PROFILE_2] = {"home.qPf2", NULL},
    [OBJ_PROFILE_3] = {"home.qPf3", NULL},
    [OBJ_PROFILE_4] = {"home.qPf4", NULL},
    [OBJ_PROFILE_5] = {"home.qPf5", NULL},
    [OBJ_COUNT] = {NULL, NULL}  // Terminator entry
};

static void splash_timer_cb(lv_timer_t * timer)
{
    lv_obj_del(splash_screen);
    splash_screen = NULL;
    lv_timer_del(timer);
    splash_timer = NULL;

    // Create main UI
    tv = lv_tabview_create(lv_scr_act());
    lv_tabview_set_tab_bar_position(tv, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tv, NAV_BAR_HEIGHT);
    lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));

    // Create navigation bar
    create_nav_bar(tv);

    // Create pages
    lv_obj_t *t1 = lv_tabview_add_tab(tv, "Home");
    lv_obj_t *t2 = lv_tabview_add_tab(tv, "Brew");
    lv_obj_t *t3 = lv_tabview_add_tab(tv, "Plot");
    lv_obj_t *t4 = lv_tabview_add_tab(tv, "Clean");
    lv_obj_t *t5 = lv_tabview_add_tab(tv, "Settings");

    create_home_screen(t1);
    create_brew_screen(t2);
    create_plot_screen(t3);
    create_clean_screen(t4);
    create_settings_screen(t5);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void create_splash_screen(lv_obj_t * parent)
{
    // Create logo image
    // LV_IMG_DECLARE(splash);  // You'll need to add this image
    // lv_obj_t * logo = lv_image_create(parent);
    // lv_image_set_src(logo, &splash);
    // lv_obj_center(logo);

    // Create version label
    lv_obj_t * version = lv_label_create(parent);
    lv_label_set_text(version, "v1.0.0");  // Replace with actual version
    lv_obj_center(version);
    // lv_obj_align_to(version, logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

static void create_nav_bar(lv_obj_t * parent)
{
    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(parent);
    lv_obj_set_style_border_width(tab_btns, 0, 0);
}

static void temp_set_value(void * indic, int32_t v)
{
    printf("STUB: temp_set_value: %d\n", v);
    //lv_meter_set_indicator_value(temp_gauge, indic, v);
}

static void create_home_screen(lv_obj_t * parent)
{
    /*Create a container with ROW flex direction*/
    lv_obj_t * cont_row = lv_obj_create(parent);
    lv_obj_set_size(cont_row, LV_PCT(100), LV_PCT(100));
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    // Create profile list
    // First item in the row (left half)
    lv_obj_t * list = lv_list_create(cont_row);
    lv_obj_set_size(list, LV_PCT(48), LV_PCT(100));
    lv_obj_set_style_pad_all(list, 10, 0);

    // Add some example profiles + add to obj_lut
    obj_lut[OBJ_PROFILE_1].obj = lv_list_add_text(list, "Profile 1");
    obj_lut[OBJ_PROFILE_2].obj = lv_list_add_text(list, "Profile 2");
    obj_lut[OBJ_PROFILE_3].obj = lv_list_add_text(list, "Profile 3");
    obj_lut[OBJ_PROFILE_4].obj = lv_list_add_text(list, "Profile 4");
    obj_lut[OBJ_PROFILE_5].obj = lv_list_add_text(list, "Profile 5");

#if 0
    // Create temperature gauge
    // Second item in the row (right half)
    temp_gauge = lv_meter_create(cont_row);
    lv_obj_set_size(temp_gauge, LV_PCT(48), 200);
    lv_obj_align(temp_gauge, LV_ALIGN_RIGHT_MID, 0, 0);
    // lv_obj_center(temp_gauge);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(temp_gauge);
    lv_meter_set_scale_ticks(temp_gauge, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(temp_gauge, scale, 8, 4, 15, lv_color_black(), 10);

    lv_meter_indicator_t * indic;

    /*Add a blue arc to the start*/
    indic = lv_meter_add_arc(temp_gauge, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(temp_gauge, indic, 0);
    lv_meter_set_indicator_end_value(temp_gauge, indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    indic = lv_meter_add_scale_lines(temp_gauge, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(temp_gauge, indic, 0);
    lv_meter_set_indicator_end_value(temp_gauge, indic, 40);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(temp_gauge, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(temp_gauge, indic, 60);
    lv_meter_set_indicator_end_value(temp_gauge, indic, 100);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(temp_gauge, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(temp_gauge, indic, 80);
    lv_meter_set_indicator_end_value(temp_gauge, indic, 100);

    /*Add a needle line indicator*/
    indic = lv_meter_add_needle_line(temp_gauge, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    temp_set_value(indic, 93);

    /*Create an animation to set the value*/
    // lv_anim_t a;
    // lv_anim_init(&a);
    // lv_anim_set_exec_cb(&a, temp_set_value);
    // lv_anim_set_var(&a, indic);
    // lv_anim_set_values(&a, 0, 100);
    // lv_anim_set_time(&a, 2000);
    // lv_anim_set_repeat_delay(&a, 100);
    // lv_anim_set_playback_time(&a, 500);
    // lv_anim_set_playback_delay(&a, 100);
    // lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    // lv_anim_start(&a);
#endif

    // Add scales button
    // TODO

    // Add brew ration
    // TODO


}

static void create_brew_screen(lv_obj_t * parent)
{
    // Create brew settings list
    lv_obj_t * list = lv_list_create(parent);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(list, 10, 0);

    // Add brew settings
    lv_list_add_text(list, "Temperature");
    lv_list_add_text(list, "Pressure");
    lv_list_add_text(list, "Flow Rate");
    lv_list_add_text(list, "Shot Time");
}

static void create_plot_screen(lv_obj_t * parent)
{
    #define MAX_PRESSURE (10)
    #define MAX_TEMP (100)
    #define WIDTH_PCT (88)
    #define HEIGHT_PCT (90)

    // Create the chart
    lv_obj_t * chart = lv_chart_create(parent);
    lv_obj_set_size(chart, lv_pct(WIDTH_PCT), lv_pct(HEIGHT_PCT));
    lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 10); // 10px from top
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, MAX_PRESSURE);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, MAX_TEMP);
    lv_chart_set_point_count(chart, 60);
    lv_obj_set_style_radius(chart, 0, 0);
    lv_chart_set_div_line_count(chart, MAX_PRESSURE + 1, 7); // 7 = 6 x 10 seconds div + 1 line

    // Create pressure scale (left side)
    lv_obj_t * scale_pressure = lv_scale_create(parent);
    lv_scale_set_mode(scale_pressure, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_obj_set_size(scale_pressure, 25, lv_pct(HEIGHT_PCT));
    lv_scale_set_range(scale_pressure, 0, MAX_PRESSURE);
    lv_scale_set_total_tick_count(scale_pressure, MAX_PRESSURE + 1);
    lv_scale_set_major_tick_every(scale_pressure, 1);
    lv_obj_set_style_pad_ver(scale_pressure, lv_chart_get_first_point_center_offset(chart) + 2, 0);
    lv_obj_align_to(scale_pressure, chart, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    // Create temperature scale (right side)
    lv_obj_t * scale_temp = lv_scale_create(parent);
    lv_scale_set_mode(scale_temp, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_obj_set_size(scale_temp, 25, lv_pct(HEIGHT_PCT));
    lv_scale_set_range(scale_temp, 0, MAX_TEMP);
    lv_scale_set_total_tick_count(scale_temp, MAX_TEMP/10 + 1);
    lv_scale_set_major_tick_every(scale_temp, 1);
    lv_obj_set_style_pad_ver(scale_temp, lv_chart_get_first_point_center_offset(chart) + 2, 0);
    lv_obj_align_to(scale_temp, chart, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    // Create bottom scale
    lv_obj_t * scale_bottom = lv_scale_create(parent);
    lv_scale_set_mode(scale_bottom, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_obj_set_size(scale_bottom, lv_pct(WIDTH_PCT), 25);
    lv_scale_set_range(scale_bottom, 0, 60); // 0 -> 60 seconds
    lv_scale_set_total_tick_count(scale_bottom, 6+1);
    lv_scale_set_major_tick_every(scale_bottom, 1);
    lv_obj_set_style_pad_hor(scale_bottom, lv_chart_get_first_point_center_offset(chart) + 2, 0);
    lv_obj_align_to(scale_bottom, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    // Add labels for the scales
    lv_obj_t * pressure_label = lv_label_create(parent);  // Create in parent instead of scale
    lv_label_set_text(pressure_label, "BAR");
    lv_obj_align_to(pressure_label, scale_pressure, LV_ALIGN_OUT_TOP_MID, 0, -5);  // Position above scale

    lv_obj_t * temp_label = lv_label_create(parent);  // Create in parent instead of scale
    lv_label_set_text(temp_label, "°C");
    lv_obj_align_to(temp_label, scale_temp, LV_ALIGN_OUT_TOP_MID, 0, -5);  // Position above scale
}

static void create_clean_screen(lv_obj_t * parent)
{
    // Create buttons container
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(cont, 20, 0);

    // Create flush button
    lv_obj_t * flush_btn = lv_button_create(cont);
    lv_obj_set_size(flush_btn, 200, 50);
    lv_obj_t * flush_label = lv_label_create(flush_btn);
    lv_label_set_text(flush_label, "Flush");

    // Create descale button
    lv_obj_t * descale_btn = lv_button_create(cont);
    lv_obj_set_size(descale_btn, 200, 50);
    lv_obj_t * descale_label = lv_label_create(descale_btn);
    lv_label_set_text(descale_label, "Descale");
}

static void create_settings_screen(lv_obj_t * parent)
{
    // Create tabview for settings
    lv_obj_t * settings_tv = lv_tabview_create(parent);
    lv_tabview_set_tab_bar_position(settings_tv, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(settings_tv, 40);
    lv_obj_set_size(settings_tv, LV_PCT(100), LV_PCT(100));

    // Create temperature tab
    lv_obj_t * temp_tab = lv_tabview_add_tab(settings_tv, "Temperature");
    lv_obj_t * temp_list = lv_list_create(temp_tab);
    lv_obj_set_size(temp_list, LV_PCT(100), LV_PCT(100));
    lv_list_add_text(temp_list, "PID Settings");
    lv_list_add_text(temp_list, "Temperature Offset");
    lv_list_add_text(temp_list, "Steam Temperature");

    // Create system tab
    lv_obj_t * sys_tab = lv_tabview_add_tab(settings_tv, "System");
    lv_obj_t * sys_list = lv_list_create(sys_tab);
    lv_obj_set_size(sys_list, LV_PCT(100), LV_PCT(100));
    lv_list_add_text(sys_list, "Display Brightness");
    lv_list_add_text(sys_list, "Units");
    lv_list_add_text(sys_list, "Language");
    lv_list_add_text(sys_list, "About");
}

static void create_popup_window(void) {
    // Create popup window (initially hidden)
    popup_window = lv_obj_create(lv_screen_active());
    lv_obj_set_size(popup_window, 400, 200);
    lv_obj_center(popup_window);
    lv_obj_add_flag(popup_window, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(popup_window);  // Ensure popup is on top

    // Set up flex layout for popup window
    lv_obj_set_flex_flow(popup_window, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(popup_window, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(popup_window, 10, 0);  // Add vertical spacing between elements

    // Create the message label
    lv_obj_t * label = lv_label_create(popup_window);
    lv_label_set_text(label, "");
    lv_obj_set_width(label, LV_PCT(100));  // Make label take full width
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);  // Center text horizontally
    obj_lut[OBJ_POPUP_MSG].obj = label;

    // Create close button
    lv_obj_t * btn = lv_button_create(popup_window);
    lv_obj_set_size(btn, 80, 30);
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Close");
    lv_obj_center(btn_label);
    lv_obj_add_event(btn, close_modal_cb, LV_EVENT_CLICKED, NULL);
}

/**
 * Callback for the close button
 */
static void close_modal_cb(lv_event_t * e) {
    lv_gaggiuino_hide_popup();
}

static lv_obj_t * find_object(const char* object) {
    for (int i = 0; obj_lut[i].name != NULL; i++) {
        if (strcmp(obj_lut[i].name, object) == 0) {
            return obj_lut[i].obj;
        }
    }
    return NULL;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_gaggiuino_ui_init(void)
{
    // Initialize default theme
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);

    // Create splash screen
    splash_screen = lv_obj_create(lv_screen_active());
    lv_obj_set_size(splash_screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(splash_screen, lv_color_hex(0xFFFFFF), 0);
    create_splash_screen(splash_screen);

    // Create popup window
    create_popup_window();

    // Create timer to switch to main UI
    splash_timer = lv_timer_create(splash_timer_cb, SPLASH_DISPLAY_TIME, NULL);
}

/**
 * Update the text of an object
 * @param object The object to update
 * @param text The text to update the object with
 */
void lv_gaggiuino_update_text(const char* object, const char* text) {
    printf("Updating text for object: %s to %s\n", object, text);
    lv_obj_t * obj = find_object(object);
    if (obj != NULL) {
        lv_label_set_text(obj, text);
    }
}

/**
 * Show a modal message window
 * @param message The message to display
 */
void lv_gaggiuino_show_popup(void) {
    lv_obj_remove_flag(popup_window, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(popup_window);  // Ensure popup is on top when shown
}

/**
 * Hide the modal message window
 */
void lv_gaggiuino_hide_popup(void) {
    lv_obj_add_flag(popup_window, LV_OBJ_FLAG_HIDDEN);
}

/**
 * Show a page
 * @param page The page to show
 */
void lv_gaggiuino_show_page(const char *page)
{
    if (strcmp(page, "popupMSG") == 0)
    {
        lv_gaggiuino_show_popup();
    }
}