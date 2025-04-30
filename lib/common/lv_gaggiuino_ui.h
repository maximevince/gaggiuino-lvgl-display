/**
 * @file lv_gaggiuino_ui.h
 * Gaggiuino UI header
 */

#ifndef LV_GAGGIUINO_UI_H
#define LV_GAGGIUINO_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/**
 * Initialize the Gaggiuino UI
 */
void lv_gaggiuino_ui_init(void);

/**
 * Update the text of an object
 * @param object The object to update
 * @param text The text to update the object with
 */
void lv_gaggiuino_update_text(const char* object, const char* text);

/**
 * Show a modal message window
 * @param message The message to display
 */
void lv_gaggiuino_show_popup(void);

/**
 * Hide the modal message window
 */
void lv_gaggiuino_hide_popup(void);

/**
 * Show a page
 * @param page The page to show
 */
void lv_gaggiuino_show_page(const char* page);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GAGGIUINO_UI_H*/ 