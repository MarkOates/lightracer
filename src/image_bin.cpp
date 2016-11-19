#include "image_bin.h"


#include <algorithm>
#include <allegro5/allegro_memfile.h>



#ifdef WIN32
	#undef strcpy
	#define strcpy strcpy_s
	#undef strcat
	#define strcat strcat_s
	#undef strncpy
	#define strncpy strncpy_s
	#undef strncat
	#define strncat strncat_s
#endif



struct ImageRecord
{
	char filename[64];
	ALLEGRO_BITMAP *image;
};

/**********************/

static const int IMAGE_NO_INDEX = -1;
static ALLEGRO_BITMAP *default_image = NULL;
ImageBin *primary_image_bin;

static inline bool image_sort_algorithm(const ImageRecord *r1, const ImageRecord *r2)
{
	return (strncmp(r1->filename, r2->filename, 64)<0);
}

static inline void sort_images(vector<ImageRecord *> &bin)
{
	std::sort(bin.begin(), bin.end(), image_sort_algorithm);
}

static inline int get_index(vector<ImageRecord *> *bin, const char *filename)
{
	int first, last, mid, comp;

	if (bin->empty()) return IMAGE_NO_INDEX;

	first = 0;
	last = bin->size()-1;

	while (first <= last)
	{
		mid = (first + last) / 2;  // compute mid point.
		comp = strcmp(bin->at(mid)->filename, filename);

		if (comp < 0) first = mid + 1;  // repeat search in top half.
		else if (comp > 0) last = mid - 1; // repeat search in bottom half.
		else return mid;     // found it. return position
	}

	return IMAGE_NO_INDEX;
}

// this function loads and appends an image into the bin and sorts the bin.
// there are no checks in this function if the image already exists in the bin.
static inline ALLEGRO_BITMAP *load_image(vector<ImageRecord *> *bin, const char *path, const char *filename)
{
	bin->push_back(new ImageRecord());
	char full_filename[64];
	strcpy(bin->back()->filename, filename);

	strncpy(full_filename, path, 64);
	strncat(full_filename, filename, 64);
	bin->back()->image = al_load_bitmap(full_filename);

	if (!bin->back()->image)
	{
		bin->back()->image = default_image;
		//std::cout << "(!) could not load image \"" << filename << "\"." << std::endl;
	}
	else
	{
		//std::cout << " -  loaded image \"" << filename << "\"." << std::endl;
	}
	ALLEGRO_BITMAP *img = bin->back()->image;
	sort_images(*bin);
	return img;
}

static inline ALLEGRO_BITMAP *load_scaled(const char *filename, int scale)
{
	int prev_flags = al_get_new_bitmap_flags();
	ALLEGRO_BITMAP *result = NULL;

	al_set_new_bitmap_flags(NULL);
	result = al_load_bitmap(filename);
	al_set_new_bitmap_flags(prev_flags);
	if (!result)
	{
		//std::cout << "(!) could not load tile \"" << filename << "\"" << std::endl;
		return NULL;
	}

	ALLEGRO_BITMAP *scaled = al_create_bitmap(al_get_bitmap_width(result)*scale,
		al_get_bitmap_height(result)*scale);
	ALLEGRO_BITMAP *prev;
	prev = al_get_target_bitmap();
	al_set_target_bitmap(scaled);
	al_clear_to_color(al_map_rgba_f(0,0,0,0));
	al_draw_scaled_bitmap(result, 0, 0,
		al_get_bitmap_width(result), al_get_bitmap_height(result), 0, 0,
		al_get_bitmap_width(result)*scale, al_get_bitmap_height(result)*scale, NULL);
	al_set_target_bitmap(prev);
	al_destroy_bitmap(result);
	result = scaled;

	//std::cout << " -  loaded scaled image \"" << filename << "\"" << std::endl;

	return scaled;
}

static bool create_default_image();
static bool first_bin_created = false;

/**********************/

ImageBin::ImageBin(const char path[64])
{
	clear_images();
	strcpy(this->path, path);
	if (!first_bin_created)
	{
		create_default_image();
	}
	primary_image_bin = this;
}

ImageBin::~ImageBin()
{
	clear_images();
}

bool ImageBin::preload_image(const char *filename)
{
	if (load_image(&image, path, filename) != default_image) return true;
	return false;
}

bool ImageBin::preload_image_scaled(const char *filename, int scale)
{
	int index = get_index(&image, filename);
	if (index == IMAGE_NO_INDEX)
	{
		char full_filename[64];
		image.push_back(new ImageRecord());
		strcpy(full_filename, this->path);
		strcat(full_filename, filename);
		strcpy(image.back()->filename, filename);

		image.back()->image = load_scaled(full_filename, scale);

		if (!image.back()->image)
		{
			image.back()->image = default_image;
			//std::cout << " !-- could not load scaled image \"" << filename << "\"." << std::endl;
		}
		else
		{
			//std::cout << " - loaded scaled image \"" << filename << "\"." << std::endl;
		}
		//ALLEGRO_BITMAP *img = image.back()->image;
		sort_images(image);
		return true;
	}
	return false;
}

ALLEGRO_BITMAP *ImageBin::get_image(const char *filename)
{
	int index = get_index(&image, filename);
	if (index == IMAGE_NO_INDEX) return load_image(&image, path, filename);
	return image[index]->image;
}

bool ImageBin::destroy_image(const char *filename)
{
	int index = get_index(&image, filename);
	if (index == IMAGE_NO_INDEX) return false;
	if (image[index]->image != default_image)
		al_destroy_bitmap(image[index]->image);
	image.erase(image.begin()+index);
	return true;
}

void ImageBin::clear_images()
{
	for (int i=0; i<(int)image.size(); i++)
		if (image[i]->image != default_image)
			al_destroy_bitmap(image[i]->image);
	image.clear();
}

size_t ImageBin::size()
{
	return image.size();
}

void ImageBin::draw_inventory()
{
	int x_spacing = 40;
	for (int i=0; i<(int)image.size(); i++)
		al_draw_bitmap(image[i]->image, i*x_spacing, 20, NULL);
}

/**********************/

static bool create_default_image()
{
	if (default_image) return true;

	// default image, in png format:
	unsigned char default_img[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A,
		0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00,
		0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x08, 0x06, 0x00, 0x00, 0x00,
		0x3B, 0xD6, 0x95, 0x4A, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58,
		0x74, 0x53, 0x6F, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x41,
		0x64, 0x6F, 0x62, 0x65, 0x20, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x52,
		0x65, 0x61, 0x64, 0x79, 0x71, 0xC9, 0x65, 0x3C, 0x00, 0x00, 0x00,
		0xB3, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA, 0xA4, 0x53, 0x31, 0x0E,
		0x83, 0x30, 0x0C, 0x8C, 0x51, 0xB6, 0xCE, 0x5D, 0x3A, 0x75, 0xE0,
		0x09, 0x4C, 0xFC, 0x5F, 0x9D, 0x78, 0x42, 0x17, 0x16, 0x3A, 0x30,
		0x33, 0x1B, 0x8C, 0xB8, 0xE8, 0x62, 0x82, 0x04, 0xF4, 0xA4, 0x28,
		0xC9, 0xD9, 0x67, 0x27, 0x8E, 0x23, 0xAA, 0x1A, 0x3C, 0x44, 0x64,
		0x47, 0x2E, 0x7E, 0xE2, 0xB9, 0x58, 0x12, 0x4D, 0xE3, 0xEB, 0x30,
		0x20, 0x07, 0x11, 0x64, 0x36, 0x63, 0x49, 0xE4, 0xF1, 0x78, 0x0E,
		0x29, 0x40, 0xE5, 0x85, 0x66, 0xB4, 0xE1, 0x05, 0xE0, 0xCC, 0x2F,
		0x5D, 0x6B, 0xCB, 0x6C, 0x62, 0xC5, 0xCC, 0xEB, 0x23, 0xCE, 0x74,
		0xB2, 0x11, 0xE1, 0x2A, 0xEC, 0x24, 0x91, 0x37, 0x67, 0x81, 0x64,
		0xD1, 0x3D, 0x07, 0x8A, 0x17, 0xF8, 0x09, 0x79, 0x6F, 0x6B, 0x60,
		0x2D, 0xD8, 0xAF, 0x7F, 0x5F, 0x3A, 0x32, 0xFC, 0xAB, 0xF0, 0x0F,
		0x50, 0xED, 0x6F, 0xD7, 0xA2, 0x8A, 0x69, 0x06, 0x98, 0x27, 0xBF,
		0x3C, 0xF3, 0x62, 0xC8, 0xEE, 0xE4, 0x3A, 0x6C, 0xB5, 0x67, 0x1C,
		0x77, 0x18, 0x8C, 0x75, 0xF3, 0xD9, 0x89, 0xD9, 0x86, 0x0E, 0x13,
		0x57, 0x55, 0x65, 0x47, 0x06, 0x02, 0x16, 0x7B, 0xFB, 0xCE, 0xAF,
		0x9A, 0x05, 0x18, 0x00, 0xE7, 0x13, 0x9E, 0x16, 0xD5, 0x9E, 0x99,
		0x41, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42,
		0x60, 0x82 };

	//ALLEGRO_FILE *file = al_open_memfile(sizeof(default_img), default_img); // <-- 4.9.22
	ALLEGRO_FILE *file = al_open_memfile(default_img, sizeof(default_img), "rw"); // <-- since 5.0.0 RC4
	//if (!file) std::cout << "memfile loading didn't work :(" << std::endl;

	default_image = al_load_bitmap_f(file, ".png");
	//if (!default_image) std::cout << "loading default_image didn't work :(" << std::endl;
	//else
		al_lock_bitmap(default_image, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

	al_fclose(file);

	return true;
}

void ImageBin::set_path(const char *path)
{
	strcpy(this->path, path);
}


ImageBin *get_image_bin_instance()
{
	if (!primary_image_bin) primary_image_bin = new ImageBin("data/images/");
	return primary_image_bin;
}


/**********************/

ALLEGRO_BITMAP *get_image(const char *filename)
{
	return get_image_bin_instance()->get_image(filename);
}

bool reload_image(const char *filename)
{
	get_image_bin_instance()->destroy_image(filename);
	return get_image_bin_instance()->preload_image(filename);
}

void set_image_path(const char *path)
{
	get_image_bin_instance()->set_path(path);
	return;
}

bool preload_image_scaled(const char *filename, int scale)
{
	return get_image_bin_instance()->preload_image_scaled(filename, scale);
	return false;
}