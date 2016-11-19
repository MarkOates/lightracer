#ifndef __PROFILING_TIMER_HEADER
#define __PROFILING_TIMER_HEADER





#include <string>
#include <allegro5\allegro.h>
#include <allegro5\allegro_font.h>

typedef char char64_t[64];

#define PROFILE_TIMER_TIME_T double

void init_profiling();
void start_profile_timer(const char64_t name);
void stop_profile_timer(const char64_t name);
PROFILE_TIMER_TIME_T get_profile_timer_length(const char64_t name);
void draw_timer_profile_graph(float x, float y, ALLEGRO_FONT *f);




#endif