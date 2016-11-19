


#include "font_bin.h"
//#include <iostream>
#include <algorithm>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

typedef char char64_t[64];

#define FONT_BIN_SIZE 32

struct FontRecord
{
	char64_t filename;
	int size;
	ALLEGRO_FONT *font;
	bool in_use;
};

class FontBin
{
private:
	char path[256];
	int _num_fonts;
	bool sorted;
	int linear_search(const char64_t filename, int size);
	int binary_search(const char64_t filename, int size);
	int get_index(const char64_t filename, int size);
	ALLEGRO_FONT *_load(const char64_t filename, int size);
	FontRecord font[FONT_BIN_SIZE];
	int get_unused_record();
//	ALLEGRO_FONT *default_font;

public:
	//void create_default_font();

	FontBin(const char *path="");
	~FontBin();

	void set_load_flags(int flags);
	bool load(const char64_t filename, int size);

	ALLEGRO_FONT *get(const char64_t filename, int size);
	bool erase(const char64_t filename, int size);

	ALLEGRO_FONT *get_default_font();
	void print_font_list();
	void set_path(const char *path="");
	void clear();
	void sort();
};



static char freestr[256];
static int comp, first, last, mid, freei, load_flags;
static ALLEGRO_FONT *default_font = NULL;
static FontBin *primary_font_bin = NULL;

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

static void create_default_font();

FontBin::FontBin(const char *path)
{
	//default_font = NULL;
	for (int i=0; i<FONT_BIN_SIZE; i++)
	{
		strcpy(font[i].filename, "\0");
		font[i].font = NULL;
		font[i].in_use = false;
		font[i].size = 0;
	}
	sorted = false;
	_num_fonts = 0;
	load_flags = NULL;
	
	if (!default_font) create_default_font();
	primary_font_bin = this;

	set_path(path);
}

FontBin::~FontBin()
{
	//todo
}

void FontBin::set_path(const char *path)
{
	strcpy(this->path, path);
}


int FontBin::linear_search(const char64_t filename, int size)
{
	for (int i=0; i<_num_fonts; i++)
		if (strcmp(font[i].filename, filename)==0 && font[i].size==size && font[i].in_use) return i;
	return -1;
}


static int int_comp(int i1, int i2)
{
	if (i1 == i2) return 0;
	if (i1 < i2) return -1;
	else return 1;
}


int FontBin::binary_search(const char64_t filename, int size)
{
	first = 0;
	last = FONT_BIN_SIZE;

	while (first <= last)
	{
		mid = (first + last) / 2;  // compute mid point.
		comp = strcmp(font[mid].filename, filename);
		if (comp==0) comp = int_comp(font[mid].size, size); // if the filenames match, check font size

		if (comp < 0) 
			first = mid + 1;  // repeat search in top half.
		else if (comp > 0) 
			last = mid - 1; // repeat search in bottom half.
		else
		{
			if (!font[mid].in_use) return -1;
			return mid;     // found it. return position
		}
	}
	return -1;
}

int FontBin::get_index(const char64_t filename, const int size)
{
	//int last_loc
	for (int i=0; i<FONT_BIN_SIZE; i++)
	{
		if ((strcmp(font[i].filename, filename)==0)) return i;
	}
	return -1;
}


ALLEGRO_FONT *FontBin::get(const char64_t filename, const int size)
{
	// find the index
	if (sorted) freei = binary_search(filename, size);
	else freei = linear_search(filename, size);

	// if there is no font record, load the image
	if (freei == -1)
		return _load(filename, size);

	//std::cout << "here: \"" << filename << "\"";

	// return the default font if the ALLEGRO_FONT is invalid
	if (!font[freei].font) return default_font;
	return font[freei].font;
}


bool FontBin::load(const char64_t filename, const int size)
{
	if (get(filename, size) == default_font) return false;
	return true;
}


void FontBin::set_load_flags(int flags)
{
	load_flags = flags;
}


bool FontBin::erase(const char64_t filename, const int size)
{
	int index = get_index(filename, size);
	if (index == -1)
	{
		//std::cout << "(!) could not erase font \"" << filename << "\" at size " << size << "." << std::endl;
		return false;
	}
	
	strcpy(font[index].filename, "\0");
	al_destroy_font(font[index].font);
	font[index].in_use = false;
	font[index].size = 0;

	//std::cout << "erased font \"" << filename << "\" at size " << size << "." << std::endl;
	
	return true;
}



bool fontRecordPresidence(const FontRecord& a, const FontRecord& b) {
	int comp = strcmp(a.filename, b.filename);
	if (comp==0) return a.size < b.size;
    else return (comp<0);
}

void FontBin::sort()
{
	int elements = sizeof(font) / sizeof(font[0]); 
	std::sort(font, font+elements, fontRecordPresidence);
	sorted = true;
}

int FontBin::get_unused_record()
{
	// note: this function will return 0 if a record is not found,
	// it is assumed that this function will not be called
	// if _num_fonts >= FONT_BIN_SIZE
	for (int i=0; i<FONT_BIN_SIZE; i++)
		if (!font[i].in_use) return i;
	return 0;
}

ALLEGRO_FONT *FontBin::_load(const char64_t filename, const int size)
{
	if (_num_fonts >= FONT_BIN_SIZE) return false;

	ALLEGRO_FONT *f=NULL;

	strncpy(freestr, path, 64);
	strncat(freestr, filename, 64);

	_num_fonts++;
	int index = get_unused_record(); // find an empty ImageRecord (linear search)

	strncpy(font[index].filename, filename, 64);
	font[index].font = al_load_ttf_font(freestr, size, load_flags);
	font[index].size = size;
	font[index].in_use = true;

	if (!font[index].font)
	{
		//std::cout << "(!) could not load font \"" << freestr << "\" at " << size << " font size." << std::endl;
		f = default_font;
	}
	else
	{
		//std::cout << " - loaded font \"" << freestr << "\" at " << size << " font size." << std::endl;
		f = font[index].font;
	}

	//load_flags = NULL;
	sort();
	return f;
}

void FontBin::clear()
{
	for (int i=0; i<FONT_BIN_SIZE; i++)
	{
		al_destroy_font(font[i].font);
		strcpy(font[i].filename, "\0");
		font[i].in_use = false;
		font[i].size = 0;
	}
	sorted = false;
}

ALLEGRO_FONT *FontBin::get_default_font()
{
	return default_font;
}

#include <string>
void FontBin::print_font_list()
{
	std::string str = "";
	int index = 0;
	for (int i=0; i<FONT_BIN_SIZE; i++)
	{
		index = get_index(font[i].filename, font[i].size);
		//if (!font[i].in_use) std::cout << i << "(" << index << "): [empty]" << std::endl;
		//else if (!font[i].font) std::cout << i << "(" << index << "): (!)\"" << font[i].filename << "\" size: " << font[i].size << std::endl;
		//else std::cout << i << "(" << index << "): \"" << font[i].filename << "\" size: " << font[i].size << std::endl;
	}
}


static inline FontBin *get_font_bin_instance()
{
	if (!primary_font_bin)
	{
		primary_font_bin = new FontBin("data/fonts/");
		create_default_font();
	}
	return primary_font_bin;
}


/*********/


ALLEGRO_FONT *get_font(const char *filename, int size)
{
	return get_font_bin_instance()->get(filename, size);
}

ALLEGRO_FONT *get_default_font()
{
	get_font_bin_instance();
	return default_font;
}


/***********/


static void create_default_font()
{
	if (default_font) return;
	//TODO this function needs to be completly internal.  see image_bin.create_default_font().
	
	default_font = al_load_font("data/default.ttf", -18, NULL);
	//if (!default_font) std::cout << "(!) CRITICAL: could not load \"data/default.ttf\"" << std::endl;
}

