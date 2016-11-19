#ifndef __FONT_BIN
#define __FONT_BIN


#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

ALLEGRO_FONT *get_font(const char *filename, int size);
ALLEGRO_FONT *get_default_font();



#endif __FONT_BIN
