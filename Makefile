


ALLEGRO_DIR=/Users/markoates/Repos/allegro5/include/

OBJS=animation event_object font_bin framework image_bin profiling timer sample_bin

PROGRAM_SOURCES := $(shell find src -name '*.cpp')




ALLEGRO_LIBS_MAIN=allegro_color allegro_font allegro_ttf allegro_memfile allegro_dialog allegro_audio allegro_acodec allegro_primitives allegro_image allegro allegro_main
ALLEGRO_LIBS_LINK_MAIN_ARGS := $(ALLEGRO_LIBS_MAIN:%=-l%)




#OBJ_FILES=$(OBJS:%=obj/%.o)

main:
	g++ $(PROGRAM_SOURCES) $(ALLEGRO_LIBS_LINK_MAIN_ARGS)



