#ifndef __SAMPLE_BIN
#define __SAMPLE_BIN



#include <allegro5/allegro_audio.h>

void set_sample_path(const char *path);
ALLEGRO_SAMPLE *get_sample(const char *identifier);



#endif __SAMPLE_BIN
