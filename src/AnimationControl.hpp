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
	AnimationControl();
	AnimationControl &set(float *val, float start, float end, double length_sec, float (*interpolation)(float));

	AnimationControl &delay(float time_sec);

	AnimationControl &easing(float (*interpolation)(float));

	AnimationControl &callback(void (*callback_func)());

	void update(double time);

	friend AnimationControl *find_control(float *val);
};

