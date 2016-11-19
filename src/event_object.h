#pragma once

#ifndef __EVENT_OBJECT_HEADER
#define __EVENT_OBJECT_HEADER


#include <algorithm>
#include <vector>


class Framework;



class uses_keyboard
{
private:
	static std::vector<uses_keyboard *> obj;

	static void update_all_on_key_up();
	static void update_all_on_key_down();
	static void update_all_on_key_char();

	friend Framework;

public:
	virtual void on_key_char() {}
	virtual void on_key_up() {}
	virtual void on_key_down() {}

	uses_keyboard();
	~uses_keyboard();
};




class uses_joystick
{
private:
	static std::vector<uses_joystick *> obj;

	static void update_all_on_joy_button_down();
	static void update_all_on_joy_button_up();
	static void update_all_on_joy_axes();

	friend Framework;

public:
	virtual void on_joy_button_down() {}
	virtual void on_joy_button_up() {}
	virtual void on_joy_axes() {}

	uses_joystick();
	~uses_joystick();
};



class uses_mouse
{
private:
	static std::vector<uses_mouse *> obj;

	static void update_all_on_mouse_up();
	static void update_all_on_mouse_down();
	static void update_all_on_mouse_axes();

	friend Framework;

public:
	virtual void on_mouse_up() {}
	virtual void on_mouse_down() {}
	virtual void on_mouse_axes() {}

	uses_mouse();
	~uses_mouse();
};




#endif