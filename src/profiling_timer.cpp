

#include "profiling_timer.h"

#include <vector>
#include <algorithm>

#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_color.h>

#include <sstream>


#ifdef WIN32
	#undef sprintf
	#define sprintf sprintf_s
	//#undef strcmp
	//#define strcmp strcmp_s
	#undef strcpy
	#define strcpy strcpy_s
#endif


#define MAX_TIMER_NAME_LEN 64
static int comp, first, last, mid, freei;


static int int_comp(int i1, int i2)
{
	if (i1 == i2) return 0;
	if (i1 < i2) return -1;
	else return 1;
}


class profile_timer_class
{
public:
	char64_t name;
	PROFILE_TIMER_TIME_T start_time;
	PROFILE_TIMER_TIME_T stop_time;
};


bool timerRecordPresidence(const profile_timer_class *a, const profile_timer_class *b) {
	return (strcmp(a->name, b->name)<0);
}


class profile_timer_manager_class
{
private:
	int num_timers;
	bool sorted;
	ALLEGRO_BITMAP *graph;

	int create_timer(const char64_t name)
	{
		timer.push_back(new profile_timer_class());
		timer.back()->start_time = 0;
		timer.back()->stop_time = 0;
		strcpy(timer.back()->name, name);
		num_timers++;

		std::sort(timer.begin(), timer.end(), timerRecordPresidence);
		sorted = true;

		return num_timers-1;
	}

	int linear_search(const char64_t name)
	{
		for (int i=0; i<num_timers; i++)
			if (strcmp(timer[i]->name, name)==0) return i;
		return -1;
	}

	int binary_search(const char64_t name)
	{
		return linear_search(name);
		//todo:

		first = 0;
		last = timer.size();

		while (first <= last)
		{
			mid = (first + last) / 2;  // compute mid point.
			comp = strcmp(timer[mid]->name, name);

			if (comp < 0) 
				first = mid + 1;  // repeat search in top half.
			else if (comp > 0) 
				last = mid - 1; // repeat search in bottom half.
			else
				return mid;     // found it. return position
		}
		return -1;
	}

	int get_timer_index(const char64_t name)
	{
		int index = 0;

		// find the timer
		if (!sorted) index = linear_search(name);
		else index = binary_search(name);

		// if the timer hasn't been created yet, make one
		if (index == -1) index = create_timer(name);

		return index;
	}

public:
	std::vector<profile_timer_class *> timer;

	profile_timer_manager_class()
	{
		num_timers = 0;
		timer.clear();
	}

	void clear_timers()
	{
		for (int i=0; i<(int)timer.size(); i++) delete timer[i];
		timer.clear();
		size_t size;
		size++;
	}

	void start_timer(const char64_t name)
	{
		int index = get_timer_index(name);

		// set the start time
		timer[index]->start_time = (PROFILE_TIMER_TIME_T)al_current_time();
		timer[index]->stop_time = 0;
	}

	void stop_timer(const char64_t name)
	{
		// set the end time
		timer[get_timer_index(name)]->stop_time = (PROFILE_TIMER_TIME_T)al_current_time();
	}

	double get_duration(const char64_t name)
	{
		int index = get_timer_index(name);
		return timer[index]->stop_time - timer[index]->start_time;
	}
};


profile_timer_manager_class profile_timer_manager;
ALLEGRO_COLOR bg_color, bar_color, font_color, limit_bar_color;



void init_profiling()
{
	al_init(); // in case it hasn't been initialized already
	// will need to to these in the future (current allegro version does not have al_is_font_addon_init()
	//al_init_primitives_addon();
	//al_init_font_addon();
	//al_init_ttf_addon();

	bg_color = al_color_name("black");
	bar_color = al_color_name("orange");
	font_color = al_color_name("white");
	limit_bar_color = al_color_name("lightblue");
}

void start_profile_timer(const char64_t name)
{
	profile_timer_manager.start_timer(name);
}

void stop_profile_timer(const char64_t name)
{
	profile_timer_manager.stop_timer(name);
}

PROFILE_TIMER_TIME_T get_profile_timer_length(const char64_t name)
{
	return profile_timer_manager.get_duration(name);
}

void draw_timer_profile_graph(float x, float y, ALLEGRO_FONT *f)
{
	float w = 300;
	float line_height = 25;
	float pad = 20;
	float h = profile_timer_manager.timer.size()*line_height + pad*2;
	PROFILE_TIMER_TIME_T duration = 0.0;
	int i=0;
	char time[32]; 

	al_draw_filled_rounded_rectangle(x, y, x+w, y+h, 8, 8, bg_color);

	if (!f) return;

	std::vector<profile_timer_class *>::iterator it = profile_timer_manager.timer.begin();
	for ( ; it != profile_timer_manager.timer.end(); ++it)
	{
		duration = (*it)->stop_time - (*it)->start_time;
		al_draw_filled_rectangle(x, y+pad+line_height*i+15, x+duration*10000, y+pad+line_height*i+20, al_color_name("orange"));
		al_draw_text(f, font_color, x+pad, y+pad+line_height*i, 0, (*it)->name);
		sprintf(time, "%f", duration*100);
		al_draw_text(f, font_color, x+pad+200, y+pad+line_height*i, 0, time);
		i++;
	}
	al_draw_line(x+ALLEGRO_BPS_TO_SECS(60)*10000, y, x+ALLEGRO_BPS_TO_SECS(60)*10000, y+h, limit_bar_color, 1.0);
	al_draw_line(x+ALLEGRO_BPS_TO_SECS(72)*10000, y, x+ALLEGRO_BPS_TO_SECS(72)*10000, y+h, limit_bar_color, 1.0);
}
