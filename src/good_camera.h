
class camera_class : public vec2d, public uses_keyboard
{
private:

public:
	float z;

	//animatable<float> z;
	float depth;
	vec2d center_point;
	float home_z;

	bool keyboard_controls_on;

	camera_class()
	{
		//_z = 0.0;
		//z.set(&_z);

		depth = 128;  // the bigger this number, the less apparent the depth effect
		depth = 512;  // 512 is an accurate balance to where moving right 100px feels
		              // proportionally the same distance as moving forward 100px
		home_z = depth;
		z = home_z;
		center_point = vec2d(SCREEN_W/2, SCREEN_H/2);
		keyboard_controls_on = true;
	}

	ALLEGRO_COLOR foreground_color; 

	inline float get_scale(float object_z)
	{
		float distance_from_camera = z - object_z;
		if (distance_from_camera <= 0) return 0;
		if (depth <= 0) return 0;

		return depth / distance_from_camera;
	}

	inline vec2d get_displacement(float object_z)
	{
		float scale = get_scale(object_z);
		return vec2d(-x * scale + SCREEN_HW, -y * scale + SCREEN_HH);
	}

	//inline vec3d screen_coordinates_to_world(float 
	
	
	//inline vec2d get_coordinate_on_screen(float x, float y, float object_z)
	//{
		//return vec2d(x, y) + get_scale(object_z);
		//return ;
	//}
	

	vec2d get_xy_displacement()
	{
		return vec2d(-x + SCREEN_HW, -y + SCREEN_HH);
	}

	/*
	bool zoom_to(float position=512, float duration=3.0, float (*transition_curve)(float)=&interpolator::linear)
	{
		if (z.get_num_active_animations() != 0) return false;
		if (_z == position) return false;
		z.animate(_z, position, duration, transition_curve);
		return true;
	}
	*/


	void draw_crosshair(float _x, float _y, float _z)
	{
		float scale = get_scale(_z);
		float width = 10;
		al_draw_line((_x-x)*scale - width*scale + center_point.x,
						(_y-y)*scale + center_point.y,
						(_x-x)*scale + width*scale + center_point.x,
						(_y-y)*scale + center_point.y,
						al_color_name("red"), 1.0*scale);
		al_draw_line((_x-x)*scale + center_point.x,
						(_y-y)*scale - width*scale + center_point.y,
						(_x-x)*scale + center_point.x,
						(_y-y)*scale + width*scale + center_point.y,
						al_color_name("red"), 1.0*scale);
	}

	vec2d get_point(float _x, float _y, float _z)
	{
		float scale = get_scale(_z);
		return vec2d((_x-x)*scale + center_point.x,	(_y-y)*scale + center_point.y);
	}

	void turn_on_controls()
	{
		keyboard_controls_on = true;
	}

	void turn_off_controls()
	{
		keyboard_controls_on = false;
	}

	void on_key_char()
	{
		//z.update();

		//if (z.get_num_active_animations() != 0) debug_message("camera on dolly");

		//debug_message("Camera (" + tostring(x) + ", " + tostring(y) + ", " + tostring(z) + ")");
		//debug_message("Camera Depth (" + tostring(depth) + ")");

		switch(current_event->keyboard.keycode)
		{
			case ALLEGRO_KEY_PAD_6: x += 2.5f; break;
			case ALLEGRO_KEY_PAD_4: x -= 2.5f; break;
			case ALLEGRO_KEY_PAD_8: y -= 2.5f; break;
			case ALLEGRO_KEY_PAD_2: y += 2.5f; break;

			case ALLEGRO_KEY_PAD_PLUS: z += 0.5f; break;
			case ALLEGRO_KEY_PAD_MINUS: z -= 0.5f; break;
			case ALLEGRO_KEY_PAD_ASTERISK: depth -= 1.0f; break;
			case ALLEGRO_KEY_PAD_SLASH: depth += 1.0f; break;

			//if (just_pressed(ALLEGRO_KEY_F2: draw_objects_in_3D_space = !draw_objects_in_3D_space;
		}

		/*
		if (keyboard_controls_on)
		{
			if (pressed(ALLEGRO_KEY_PAD_6)) x += 2.5f;
			if (pressed(ALLEGRO_KEY_PAD_4)) x -= 2.5f;
			if (pressed(ALLEGRO_KEY_PAD_8)) y -= 2.5f;
			if (pressed(ALLEGRO_KEY_PAD_2)) y += 2.5f;

			if (pressed(ALLEGRO_KEY_PAD_PLUS)) z += 0.5f;
			if (pressed(ALLEGRO_KEY_PAD_MINUS)) z -= 0.5f;
			if (pressed(ALLEGRO_KEY_PAD_ASTERISK)) depth -= 1.0f;
			if (pressed(ALLEGRO_KEY_PAD_SLASH)) depth += 1.0f;

			if (just_pressed(ALLEGRO_KEY_F2)) draw_objects_in_3D_space = !draw_objects_in_3D_space;
		}
		*/
	}
};
