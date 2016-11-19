
#include "event_object.h"





// uses_keyboard //

void uses_keyboard::update_all_on_key_up()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_key_up();
}

void uses_keyboard::update_all_on_key_down()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_key_down();
}

void uses_keyboard::update_all_on_key_char()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_key_char();
}

uses_keyboard::uses_keyboard()
{
	obj.push_back(this);
}

uses_keyboard::~uses_keyboard()
{
	std::vector<uses_keyboard *>::iterator it = find(obj.begin(), obj.end(), this);
	if (it != obj.end()) { delete (*it); obj.erase(it); }
}

std::vector<uses_keyboard *> uses_keyboard::obj;




// uses_mouse //

void uses_mouse::update_all_on_mouse_up()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_mouse_up();
}

void uses_mouse::update_all_on_mouse_down()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_mouse_down();
}

void uses_mouse::update_all_on_mouse_axes()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_mouse_axes();
}

uses_mouse::uses_mouse()
{
	obj.push_back(this);
}

uses_mouse::~uses_mouse()
{
	std::vector<uses_mouse *>::iterator it = find(obj.begin(), obj.end(), this);
	if (it != obj.end()) { delete (*it); obj.erase(it); }
}

std::vector<uses_mouse *> uses_mouse::obj;




// uses_joystick //

void uses_joystick::update_all_on_joy_button_down()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_joy_button_down();
}

void uses_joystick::update_all_on_joy_button_up()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_joy_button_up();
}

void uses_joystick::update_all_on_joy_axes()
{
	for (int i=0; i<(int)obj.size(); i++) obj[i]->on_joy_axes();
}

uses_joystick::uses_joystick()
{
	obj.push_back(this);
}

uses_joystick::~uses_joystick()
{
	std::vector<uses_joystick *>::iterator it = find(obj.begin(), obj.end(), this);
	if (it != obj.end()) { delete (*it); obj.erase(it); }
}

std::vector<uses_joystick *> uses_joystick::obj;
