#include "framework.h"


Framework *current_framework = NULL;
ALLEGRO_EVENT *current_event = NULL;
//object2d *current_object = NULL;



void error(const char *e) { }


Framework::Framework() :
	timer_func(NULL),
	key_up_func(NULL),
	key_down_func(NULL),
	key_char_func(NULL),
	mouse_up_func(NULL),
	mouse_down_func(NULL),
	mouse_axes_func(NULL),
	joy_up_func(NULL),
	joy_down_func(NULL),
	joy_axis_func(NULL),
	display_close_func(NULL),
	abort_program(false)
{
	int w=800,h=600;
	// init //

	if (!al_init()) error("Could not init allegro");

	// set path //

	ALLEGRO_PATH *resource_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_change_directory(al_path_cstr(resource_path, '/'));
	al_destroy_path(resource_path);

	// init components //

	if (!al_install_mouse()) error("Could not install mouse");
	if (!al_install_keyboard()) error("Could not install keyboard");
	if (!al_install_joystick()) error("Could not install joystick");
	if (!al_install_audio()) error("Could not install audio");

	al_init_font_addon();
	if (!al_init_ttf_addon()) error("Could not init ttf addon");
	if (!al_init_primitives_addon()) error("Could not init primitives addon");
	if (!al_init_image_addon()) error("Could not init image addon");
	if (!al_init_acodec_addon()) error("Could not init acodec addon");

	if (!al_reserve_samples(32)) error("Could not reserve samples");

	// set the display and bitmap settings //

	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	
	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 4, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 16, ALLEGRO_SUGGEST);	

	al_set_new_display_flags(ALLEGRO_WINDOWED);

	display = al_create_display(w, h);

	// install the timer //

	timer = al_create_timer(ALLEGRO_BPS_TO_SECS(60));

	// setup the event queue //

	queue = al_create_event_queue();

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_display_event_source(display));

	// seed the random

	srand(time(NULL));

	// set the current framework and background operations //

	current_framework = this;
}


Framework::~Framework()
{
	al_destroy_display(display);
}



int Framework::run_loop()
{
	al_clear_to_color(al_color_name("efefef"));
	al_flip_display();
	al_start_timer(timer);
	ALLEGRO_EVENT this_event, next_event;

	while (!abort_program)
	{
		current_event = NULL;
		al_wait_for_event(queue, &this_event);
		current_event = &this_event;
		switch (this_event.type)
		{
		case ALLEGRO_EVENT_TIMER:
			//al_clear_to_color(al_color_html("efefef"));
			update_animations();
			if (timer_func) timer_func();
			//draw_all_objects();
			al_flip_display();
			while (al_peek_next_event(queue, &next_event)
				&& next_event.type == ALLEGRO_EVENT_TIMER
				&& next_event.timer.source == timer)
				al_drop_next_event(queue);
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			uses_keyboard::update_all_on_key_down();
			if (key_down_func) key_down_func();
			break;
		case ALLEGRO_EVENT_KEY_UP:
			uses_keyboard::update_all_on_key_up();
			if (key_up_func) key_up_func();
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
			uses_keyboard::update_all_on_key_char();
			if (key_char_func) key_char_func();
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			uses_mouse::update_all_on_mouse_down();
			if (mouse_down_func) mouse_down_func();
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			uses_mouse::update_all_on_mouse_up();
			if (mouse_up_func) mouse_up_func();
			break;
		case ALLEGRO_EVENT_MOUSE_AXES:
			uses_mouse::update_all_on_mouse_axes();
			if (mouse_axes_func) mouse_axes_func();
			break;
		case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
			uses_joystick::update_all_on_joy_button_down();
			if (joy_down_func) joy_down_func();
			break;
		case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
			uses_joystick::update_all_on_joy_button_up();
			if (joy_up_func) joy_up_func();
			break;
		case ALLEGRO_EVENT_JOYSTICK_AXIS:
			uses_joystick::update_all_on_joy_axes();
			if (joy_axis_func) joy_axis_func();
			break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			abort_program = true;
			if (display_close_func) display_close_func();
			break;
		}
	};

	return 0;
}

