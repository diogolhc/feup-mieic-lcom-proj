#include <lcom/lcf.h>
#include "clue.h"
#include "font.h"

#define CLUE_CHAR_SPACING 10
#define CLUE_BAR_MARGIN 5
#define RECTANGLE_HEIGHT 5

int new_word_clue(word_clue_t *clue, char *word) {
    clue->word = word;
    clue->size = strlen(word);
    clue->missing = clue->size - 1;
    clue->width = clue->size * (FONT_CHAR_WIDTH + CLUE_CHAR_SPACING) - CLUE_CHAR_SPACING;
    clue->height = FONT_CHAR_HEIGHT + RECTANGLE_HEIGHT;

    clue->clue = malloc(sizeof(char) * clue->size + 1);
    if (clue->clue == NULL)
        return 1;

    memset(clue->clue, ' ', clue->size);
    clue->clue[clue->size] = '\0';
    
    return 0;
}

int word_clue_draw(word_clue_t *clue, frame_buffer_t buf, uint16_t x, uint16_t y) {
    for (size_t i = 0; i < clue->size; i++) {
        uint16_t current_x = x + i * (FONT_CHAR_WIDTH + CLUE_CHAR_SPACING);
        if (vb_draw_rectangle(buf, current_x-2, y-2, FONT_CHAR_WIDTH+4, FONT_CHAR_HEIGHT+ CLUE_BAR_MARGIN+RECTANGLE_HEIGHT +4, 0xffffff) != OK)
            return 1;

        if (vb_draw_rectangle(buf, current_x, y + FONT_CHAR_HEIGHT + CLUE_BAR_MARGIN, FONT_CHAR_WIDTH, RECTANGLE_HEIGHT, 0x000000) != OK)
            return 1;

        char letter = clue->clue[i];
        if (letter != ' ') {
            if (font_draw_char(buf, letter, current_x, y))
                return 1;
        }
    }

    return 0;
}

int word_clue_hint(word_clue_t *clue) {
    if (clue->missing <= 0)
        return 1;
    size_t hint_pos = rand() % clue->missing;
    
    for (size_t i = 0; i < clue->size; i++) {
        if (clue->clue[i] != ' ') 
            continue;

        if (hint_pos == 0) {
            clue->clue[i] = clue->word[i];
            clue->missing--;
            break;
        }
        hint_pos--;
    }

    return 0;
}

void free_word_clue(word_clue_t *clue) {
    free(clue->clue);
}