CC = gcc
OBJ_PATH = obj/
LIBRARY_PATH = lib/
SOURCE_PATH = src/
INCLUDE_PATH = include/
DOC_PATH = doc/

DEBUG_FLAGS = -g
WARNING_FLAGS = -Wall -Wextra -Wfloat-equal -Wdouble-promotion -Wswitch-default -Winit-self -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wlogical-op -Wstrict-prototypes -Wnested-externs
COMPILE_FLAGS = $(WARNING_FLAGS) # $(DEBUG_FLAGS)

vpath %.c  $(SOURCE_PATH)
vpath %.h  $(INCLUDE_PATH)
vpath %.a  $(LIBRARY_PATH)
vpath %.so $(LIBRARY_PATH)
vpath %.o  $(OBJ_PATH)
vpath libhash.a $(LIBRARY_PATH)

libhash.a : generic_hash_table.o base_hash_table.o | lib
	ar -crv $(LIBRARY_PATH)$@ $(OBJ_PATH)generic_hash_table.o $(OBJ_PATH)base_hash_table.o

generic_hash_table.o : generic_hash_table.c generic_hash_table.h
base_hash_table.o : base_hash_table.c base_hash_table.h generic_hash_table.h


%.o : | obj
	$(CC) $(COMPILE_FLAGS) -c $< -I$(INCLUDE_PATH) -o $(OBJ_PATH)$@

obj :
	mkdir -p $(OBJ_PATH)

lib :
	mkdir -p $(LIBRARY_PATH)

doc :
	doxygen

clean :
	@rm -rf $(OBJ_PATH) $(LIBRARY_PATH) $(DOC_PATH)
