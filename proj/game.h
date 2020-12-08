#ifndef __GAME_H
#define __GAME_H

#include <lcom/lcf.h>

#define GAME_BAR_COLOR 0x00dddddd
#define GAME_BAR_COLOR_DARK 0x00aaaaaa
#define GAME_BAR_HEIGHT 150
#define GAME_BAR_PADDING 5
#define GAME_BAR_INNER_HEIGHT ((GAME_BAR_HEIGHT) - (GAME_BAR_PADDING))

uint32_t game_get_selected_color();
uint16_t game_get_selected_thickness();
bool game_is_pencil_primary();
int game_set_pencil_primary();
int game_set_eraser_primary();
void game_toggle_pencil_eraser();
int game_load_assets(enum xpm_image_type type);
int game_start_round();
void game_round_timer_tick();
int draw_game_bar();

#endif /* __GAME_H */
