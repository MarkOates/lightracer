#include "AnimationControl.hpp"

#include "interpolations.h"

#include <allegro5/allegro.h>


AnimationControl::AnimationControl() : active(false), val(0), start(0), end(0),
   start_time(0), length_sec(0), interpolation(interpolator::linear)
{}

AnimationControl &AnimationControl::set(float *val, float start, float end, double length_sec, float (*interpolation)(float))
{
   this->active = true;
   this->val = val;
   this->start = start;
   this->end = end;
   this->start_time = al_get_time();
   this->length_sec = length_sec;
   this->interpolation = interpolation;
   this->callback_func = nullptr;
   //this->callback_data = NULL;
   return *this;
}

AnimationControl &AnimationControl::delay(float time_sec)
{
   this->start_time += time_sec;
   return *this;
}

AnimationControl &AnimationControl::easing(float (*interpolation)(float))
{
   this->interpolation = interpolation;
   return *this;
}

AnimationControl &AnimationControl::callback(void (*callback_func)())
{
   this->callback_func = callback_func;
   return *this;
}

void AnimationControl::update(double time)
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
