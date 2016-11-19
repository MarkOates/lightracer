


ALLEGRO_DIR=/Users/markoates/Repos/allegro5/include/

OBJS=animation event_object font_bin framework image_bin profiling timer sample_bin

OBJ_FILES=$(OBJS:%=obj/%.o)

$(OBJ_FILES): obj/%.o : src/%.cpp
	g++ -c -o $@ $< -I$(ALLEGRO_DIR)



