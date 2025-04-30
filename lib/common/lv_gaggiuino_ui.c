/**
 * @file lv_gaggiuino_ui.c
 * Gaggiuino UI implementation
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lv_gaggiuino_ui.h"
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define NAV_BAR_HEIGHT 40
#define SPLASH_DISPLAY_TIME 2000  // 2 seconds

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

// Object lookup table
static obj_lut_entry_t obj_lut[] = {
    [OBJ_POPUP_MSG] = {"popupMSG.t0", NULL},  // Will be initialized when popup is created
    [OBJ_PROFILE_1] = {"home.qPf1", NULL},
    [OBJ_PROFILE_2] = {"home.qPf2", NULL},
    [OBJ_PROFILE_3] = {"home.qPf3", NULL},
    [OBJ_PROFILE_4] = {"home.qPf4", NULL},
    [OBJ_COUNT] = {NULL, NULL}  // Terminator entry
};

static void splash_timer_cb(lv_timer_t * timer)
{
    lv_obj_del(splash_screen);
    splash_screen = NULL;
    lv_timer_del(timer);
    splash_timer = NULL;

    // Create main UI
    tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, NAV_BAR_HEIGHT);
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
    LV_IMG_DECLARE(splash);  // You'll need to add this image
    lv_obj_t * logo = lv_img_create(parent);
    lv_img_set_src(logo, &splash);
    lv_obj_center(logo);

    // Create version label
    // lv_obj_t * version = lv_label_create(parent);
    // lv_label_set_text(version, "v1.0.0");  // Replace with actual version
    // lv_obj_align_to(version, logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

static void create_nav_bar(lv_obj_t * parent)
{
    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(parent);
    lv_obj_set_style_border_width(tab_btns, 0, 0);
}

static void create_home_screen(lv_obj_t * parent)
{
    // Create temperature gauge
    lv_obj_t * gauge = lv_meter_create(parent);
    lv_obj_set_size(gauge, 200, 200);
    lv_obj_center(gauge);

    // Create profile list
    lv_obj_t * list = lv_list_create(parent);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(list, 10, 0);

    // Add some example profiles + add to obj_lut
    obj_lut[OBJ_PROFILE_1].obj = lv_list_add_text(list, "Profile 1");
    obj_lut[OBJ_PROFILE_2].obj = lv_list_add_text(list, "Profile 2");
    obj_lut[OBJ_PROFILE_3].obj = lv_list_add_text(list, "Profile 3");
    obj_lut[OBJ_PROFILE_4].obj = lv_list_add_text(list, "Profile 4");
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
    // Create chart
    lv_obj_t * chart = lv_chart_create(parent);
    lv_obj_set_size(chart, LV_PCT(100), LV_PCT(100));
    lv_chart_set_point_count(chart, 100);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 10);
    lv_chart_set_div_line_count(chart, 5, 5);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 5, 2, true, 40);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 2, true, 40);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 5, 2, true, 40);
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
    lv_obj_t * flush_btn = lv_btn_create(cont);
    lv_obj_set_size(flush_btn, 200, 50);
    lv_obj_t * flush_label = lv_label_create(flush_btn);
    lv_label_set_text(flush_label, "Flush");

    // Create descale button
    lv_obj_t * descale_btn = lv_btn_create(cont);
    lv_obj_set_size(descale_btn, 200, 50);
    lv_obj_t * descale_label = lv_label_create(descale_btn);
    lv_label_set_text(descale_label, "Descale");
}

static void create_settings_screen(lv_obj_t * parent)
{
    // Create tabview for settings
    lv_obj_t * settings_tv = lv_tabview_create(parent, LV_DIR_TOP, 40);
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
    popup_window = lv_obj_create(lv_scr_act());
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
    lv_obj_t * btn = lv_btn_create(popup_window);
    lv_obj_set_size(btn, 80, 30);
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Close");
    lv_obj_center(btn_label);
    lv_obj_add_event_cb(btn, close_modal_cb, LV_EVENT_CLICKED, NULL);
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
    splash_screen = lv_obj_create(lv_scr_act());
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
    lv_obj_clear_flag(popup_window, LV_OBJ_FLAG_HIDDEN);
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
    // lv_tabview_set_tab_act(tv, page, LV_ANIM_OFF);
    if (strcmp(page, "popupMSG") == 0)
    {
        lv_gaggiuino_show_popup();
    }
}