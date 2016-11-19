#ifndef __ALLEGRO_IMAGE_BIN_HEADER
#define __ALLEGRO_IMAGE_BIN_HEADER



#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include <vector>
#include <string>
//#include <iostream>
#include <sstream>

using std::vector;
using std::string;

struct ImageRecord;

class ImageBin
{
private:
	vector<ImageRecord *> image;
	char path[64];

public:
	ImageBin(const char *path="");
	~ImageBin();

	//void init();

	void set_path(const char *path);

	bool preload_image(const char *filename);
	bool preload_image_scaled(const char *filename, int scale);

	ALLEGRO_BITMAP *get_image(const char *filename);

	bool destroy_image(const char *filename);
	void clear_images();
	void draw_inventory();
	size_t size();
};



ALLEGRO_BITMAP *get_image(const char *filename);
bool reload_image(const char *filename);
void set_image_path(const char *filename);
bool preload_image_scaled(const char *filename, int scale=3);



#endif