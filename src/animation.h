#pragma once


#include "interpolations.h"
#include <allegro5/allegro.h>

class AnimationControl;


inline AnimationControl *find_control(float *val);


class AnimationControl
{
private:
	// basic //
	float *val;
	float start, end;
	double start_time, length_sec;
	float (*interpolation)(float);

	// advanced features would include callback, pause, id/class identifiers, pause, and delta_time //
	void (*callback_func)();
	//void *callback_data;

public:
	bool active;
	AnimationControl() : active(false), val(NULL), start(0), end(0),
		start_time(0), length_sec(0), interpolation(interpolator::linear)
	{}
	inline AnimationControl &set(float *val, float start, float end, double length_sec, float (*interpolation)(float))
	{
		this->active = true;
		this->val = val;
		this->start = start;
		this->end = end;
		this->start_time = al_get_time();
		this->length_sec = length_sec;
		this->interpolation = interpolation;
		this->callback_func = NULL;
		//this->callback_data = NULL;
		return *this;
	}

	inline AnimationControl &delay(float time_sec)
	{
		this->start_time += time_sec;
		return *this;
	}

	inline AnimationControl &easing(float (*interpolation)(float))
	{
		this->interpolation = interpolation;
		return *this;
	}

	inline AnimationControl &callback(void (*callback_func)())
	{
		this->callback_func = callback_func;
		return *this;
	}

	inline void update(double time)
	{
		if (active)
		{
			if (time >= start_time+length_sec)
			{
				active = false;
				*val = end;
				if (callback_func) callback_func();
			}
			else if (time < start_time) { /* do nothing */ }
			else *val = interpolation((time-start_time)/length_sec) * (end-start) + start;
		}
	}

	friend inline AnimationControl *find_control(float *val);
};


#include <vector>
#include <algorithm>

class AnimationManager
{
private:
	int control_index;
	std::vector<AnimationControl *> control;
	AnimationManager() : control_index(0)
	{
		for (int i=0; i<64; i++) control.push_back(new AnimationControl());
	}
	static AnimationManager *instance;
public:
	inline void update(double time=al_get_time())
	{
		std::vector<AnimationControl *>::iterator it = control.begin();
		while (it != control.end()) { (*it)->update(time); it++; }
	}
	inline AnimationControl *get_available()
	{
		// this method is kinda ghetto //
		return control[(++control_index)%control.size()];
	}

	friend inline AnimationControl *find_control(float *val);

	static AnimationManager *get_instance()
	{
		if (!instance) instance = new AnimationManager();
		return instance;
	}
};





static inline AnimationControl *find_control(float *val)
{
	for (int i=0; i<(int)AnimationManager::get_instance()->control.size(); i++)
		if (AnimationManager::get_instance()->control[i]->val == val)
			return AnimationManager::get_instance()->control[i];
	return NULL;
}



static inline AnimationControl &animate(float &val, float start, float end, double length_sec=0.4, float (*interpolation)(float)=interpolator::doubleFastIn)
{
	return AnimationManager::get_instance()->get_available()->set(&val, start, end, length_sec, interpolation);
}

static inline AnimationControl &animate_to(float &val, float end, double length_sec=0.4, float (*interpolation)(float)=interpolator::doubleFastIn)
{
	return AnimationManager::get_instance()->get_available()->set(&val, val, end, length_sec, interpolation);
}

static inline AnimationControl &animate_delta(float &val, float delta, double length_sec=0.4, float (*interpolation)(float)=interpolator::doubleFastIn)
{
	return AnimationManager::get_instance()->get_available()->set(&val, val, val+delta, length_sec, interpolation);
}



static inline void update_animations(double time=al_get_time()) { AnimationManager::get_instance()->update(time); }
static inline bool clear_animation(AnimationControl *a) { a->active = false; }
static inline bool clear_animation(float &val) { AnimationControl *a = find_control(&val); return (a) ? !(a->active=false) : true; }
