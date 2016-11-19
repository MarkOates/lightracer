#ifndef __USEFUL_HEADER
#define __USEFUL_HEADER




#include <sstream>
#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

#include "vec2d.h"



static const float FULL_ROTATION = 6.28318531;

static inline float degrees_to_radians(float deg) { return ALLEGRO_PI * deg / 180.0f; }
static inline float radians_to_degrees(float rad) { return 180.0f / ALLEGRO_PI * rad; }

static inline void draw_crosshair(float x, float y, ALLEGRO_COLOR color=al_color_name("white"))
{
	float half_size = 5;
	al_draw_line(x, y-half_size, x, y+half_size, color, 1.0);
	al_draw_line(x-half_size, y, x+half_size, y, color, 1.0);
}

static inline void draw_crosshair(vec2d &point, ALLEGRO_COLOR color=al_color_name("white"))
{
	float half_size = 5;
	al_draw_line(point.x, point.y-half_size, point.x, point.y+half_size, color, 1.0);
	al_draw_line(point.x-half_size, point.y, point.x+half_size, point.y, color, 1.0);
}

static inline float distance( float x1, float y1, float x2, float y2)
{
      return sqrt( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) ) ;
}

static inline float manhattan_distance( float x1, float y1, float x2, float y2)
{
	return x1 - x2 + y1 - y2;
}

static inline float distance( const vec2d &point1, const vec2d &point2 )
{
      return sqrt( ((point1.x - point2.x) * (point1.x - point2.x)) + ((point1.y - point2.y) * (point1.y - point2.y)) ) ;
}

static inline float random_float(float min, float max)
{
    return ((float) rand()/RAND_MAX)*(max-min) + min;
}

static inline double random_double(double min, double max)
{
    return ((double) rand()/RAND_MAX)*(max-min) + min;
}

static inline int random_int(int min, int max)
{
    return rand()%(max-min+1) + min;
}

static inline bool random_bool()
{
    return (rand()%2 == 1);
}

static inline int random_sign()
{
    if (random_bool()) return 1;
	else return -1;
}

// this random_char() function has not been tested
/*

static inline char random_char(char min=0, char max=255)
{
	return (char)(rand()%(max-min+1) + min);
}

*/

static inline unsigned char random_letter(bool lower)
{
	if (lower) return (unsigned char)(rand()%26 + 'a');
	return (unsigned char)(rand()%26 + 'A');
}


// returns a point projected onto an axis
static inline vec2d project(vec2d &point, vec2d &axis)
{
	float somethun = (point.x * axis.x + point.y * axis.y)
	               / (pow(axis.x, 2) + pow(axis.y, 2));
	return vec2d(somethun * axis.x, somethun * axis.y);
}

// reflect a point along an axis
inline static vec2d reflect(vec2d &point, const vec2d &axis)
{
    float d = point * axis;
	return point - 2 * d * axis;
}

inline static float invert_interval(float interval)
{
	return 1.0 - interval;
}

template<class T>
inline static T limit(const T &range1, const T &range2, const T &val)
{
	float min = (range1 < range2) ? range1 : range2;
	float max = (range1 > range2) ? range1 : range2;
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static inline ALLEGRO_COLOR color(const char *color_name, float alpha=1.0)
{
	ALLEGRO_COLOR col = al_color_name(color_name);
	col.a = alpha;
	col.r *= alpha;
	col.g *= alpha;
	col.b *= alpha;
	return col;
}


static inline ALLEGRO_COLOR color_hex(const char *hex, float a=1.0f)
{
	ALLEGRO_COLOR color = al_color_html(hex);
	color.a = a;
	color.r *= a;
	color.g *= a;
	color.b *= a;
	return color;
}


static inline ALLEGRO_COLOR color_name(const char *name, float a=1.0f)
{
	ALLEGRO_COLOR color = al_color_name(name);
	color.a = a;
	color.r *= a;
	color.g *= a;
	color.b *= a;
	return color;
}


static inline ALLEGRO_COLOR color(ALLEGRO_COLOR color, float a=1.0f)
{
	color.a *= a;
	color.r *= a;
	color.g *= a;
	color.b *= a;
	return color;
}


static inline bool operator ==(const ALLEGRO_COLOR &lhs, const ALLEGRO_COLOR &rhs)
{
	if (lhs.r != rhs.r) return false;
	if (lhs.g != rhs.g) return false;
	if (lhs.b != rhs.b) return false;
	if (lhs.a != rhs.a) return false;
	return true;
}


template<class T>
static inline std::string tostring(T val)
{
	std::ostringstream s;
	s << val;
	return s.str();
}


#include <vector>
static std::vector<std::string> str_explode(const std::string &delimiter, const std::string &str)
{
    std::vector<std::string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng==0)
        return arr;//no change

    int i=0;
    int k=0;
    while(i<strleng)
    {
        int j=0;
        while (i+j<strleng && j<delleng && str[i+j]==delimiter[j])
            j++;
        if (j==delleng)//found delimiter
        {
            arr.push_back(str.substr(k, i-k));
            i+=delleng;
            k=i;
        }
        else
        {
            i++;
        }
    }
    arr.push_back(str.substr(k, i-k));
    return arr;
}





#endif