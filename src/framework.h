#pragma once

#ifndef __FRAMEWORK_HEADER
#define __FRAMEWORK_HEADER


#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_memfile.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

#include "font_bin.h"
#include "image_bin.h"
#include "sample_bin.h"
#include "useful.h"
#include "vec2d.h"
//#include "position2d.h"
//#include "object2d.h"
#include "animation.h"

//#include "sound.h"
#include "event_object.h"


extern ALLEGRO_EVENT *current_event;
//extern object2d *current_object;
extern Framework *current_framework;


class Framework
{
private:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT_QUEUE *queue;

public:
	bool abort_program;
	void (*timer_func)();
	void (*mouse_axes_func)();
	void (*mouse_down_func)();
	void (*mouse_up_func)();
	void (*key_down_func)();
	void (*key_up_func)();
	void (*key_char_func)();
	void (*display_close_func)();
	void (*joy_up_func)();
	void (*joy_down_func)();
	void (*joy_axis_func)();

	Framework();
	~Framework();
	int run_loop();
};






#endif