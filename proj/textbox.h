#ifndef __TEXTBOX_H
#define __TEXTBOX_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include "graphics.h"
#include "keyboard.h"

/** @file 
 * @brief File dedicated to text boxes.
 */

/** @defgroup textbox textbox
 * @{
 *
 * @brief Module dedicated to text boxes.
 */

#define TEXT_BOX_GUESSER_X 30 /**< @brief Guesser text box x coordinate */
#define TEXT_BOX_GUESSER_Y 700 /**< @brief Guesser text box y coordinate */
#define TEXT_BOX_GUESSER_DISPLAY_SIZE 13 /**< @brief Guesser text box display size (in chars). */

typedef int (*text_box_action)(char *); /**< @brief Text box action when ENTER is pressed. */

/**
 * @brief Enumerated type for specifying the state of a text box.
 * 
 */
typedef enum text_box_state {
    TEXT_BOX_NORMAL, /*!< Text box is in normal/base state. */
    TEXT_BOX_HOVERING, /*!< Text box is not selected and the cursor is hovering it. */
    TEXT_BOX_SELECTED_HOVERING, /*!< Text box is selected and the cursor is hovering it. */
    TEXT_BOX_SELECTED_NOT_HOVERING, /*!< Text box is selected but the cursor is not hovering it. */
    TEXT_BOX_PRESSING /*!< Text box is being pressed. */
} text_box_state;

/**
 * @brief Text box info.
 * 
 */
typedef struct text_box_t {
    char *   word; /*!< @brief Address of memory of the content of the text box. */
    uint8_t  word_size; /*!< @brief Content size (not counting with the '\0'). */
    uint16_t x; /*!< @brief Left most x coordinate of the text box. */
    uint16_t y; /*!< @brief Top most y coordinate of the text box. */
    uint8_t  cursor_pos; /*!< @brief Cursor position relative to its content start. */
    uint8_t  select_pos; /*!< @brief Position, relative to content start, from where the content is being selected (== cursor_pos if nothing selected). */
    uint8_t  start_display; /*!< @brief First position dispalyed in the text box. */
    uint8_t  display_size; /*!< @brief Number of chars displayed at the same time. */
    text_box_state state; /*!< @brief State of the text box. */
    bool     visible_cursor; /*!< @brief True if the text cursor is visible and false otherwise. */
    text_box_action action; /*!< @brief Action to perform when ENTER is pressed. */
} text_box_t;

/**
 * @brief Initiates the content of a new text box.
 * 
 * @param text_box address of memory of the text box to be initialized
 * @param x text box x coordinate
 * @param y text box y coordinate
 * @param display_size text box display size
 * @param action text box action
 * @return Return 0 upon success and non-zero otherwise
 */
int new_text_box(text_box_t *text_box, uint16_t x, uint16_t y, uint8_t display_size, text_box_action action);

/**
 * @brief Frees the space allocated in memory to store the content of a given text box
 * 
 * @param text_box address of memory of the text box
 */
void delete_text_box(text_box_t *text_box);

/**
 * @brief Clears the content of a given text box.
 * 
 * @param text_box address of memory of the text box
 * @return Return 0 upon success and non-zero otherwise
 */
int text_box_clear(text_box_t *text_box);

/**
 * @brief Switches the visibility of the text cursor of a given text box.
 * 
 * @param text_box address of memory of the text box
 */
void text_box_cursor_tick(text_box_t *text_box);

/**
 * @brief Draws a given text box to the given buffer.
 * 
 * @param buf buffer
 * @param text_box text box to draw
 * @return Return 0 upon success and non-zero otherwise
 */
int text_box_draw(frame_buffer_t buf, text_box_t text_box);

/**
 * @brief Checks if the given coordinates are inside the limits of a given text box.
 * 
 * @param text_box text box
 * @param x x coordinate
 * @param y y coordinate
 * @return Return true if the coordinates are inside the text box and false otherwise
 */
bool text_box_is_hovering(text_box_t text_box, uint16_t x, uint16_t y);

/**
 * @brief Updates the state of a given text box according to given mouse info.
 * 
 * @param text_box address of memory of the text box
 * @param hovering text box is being hovered
 * @param lb is left button pressed
 * @param rb is right button pressed
 * @param x x coordinate
 * @param y y coordinate
 * @return Return 0 upon success and non-zero otherwise
 */
int text_box_update_state(text_box_t *text_box, bool hovering, bool lb, bool rb, uint16_t x, uint16_t y);

/**
 * @brief Updates a given text box according to a keyboard event.
 * 
 * @param text_box address of memory of the text box
 * @param kbd_event keyboard event
 * @return Return 0 upon success and non-zero otherwise
 */
int text_box_react_kbd(text_box_t *text_box, kbd_event_t kbd_event);

/**
 * @brief Frees the space allocated in memory to store the clip board.
 * 
 */
void text_box_clip_board_exit();

/**@}*/

#endif /* __TEXTBOX_H */
