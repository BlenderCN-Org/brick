CC=g++
SRC_DIR=src/
CFLAGS=-I$(SRC_DIR) -I$(SRC_DIR)/math
CFLAGS+=-I/usr/include/libxml2/
CFLAGS+=$(shell pkg-config freetype2 --cflags)

LDFLAGS=$(shell pkg-config freetype2 --libs)
LDFLAGS+=-lxml2

TARGET=brick

SRCS=audio/sdl-audio.cc collision.cc data_feed/data_feed.cpp debug/console.cpp
SRCS+=event/create-force.cc event/event.cc event/sequence.cc event/serial.cc
SRCS+=fluid2-d.cc graphics/font.cpp graphics/glutility.cc graphics/material.cpp
SRCS+=graphics/render_shepherd.cpp graphics/shader.cc graphics/static_mesh.cpp
SRCS+=graphics/texture.cc input/game_controller.cpp level.cc math/function.cpp
SRCS+=math/math-utility.cc math/perlin.cc object/container.cc object/crate.cc
SRCS+=object/foliage/plant.cpp object/level-platform.cc object/object.cc
SRCS+=object/player-pet.cc object/player.cc particles/particle-manager.cc
SRCS+=object/particle-system.cc object/particle.cc physics/physics_instance.cpp
SRCS+=physics/physics_shape.cpp player-controller.cc ui/hud.cpp
SRCS+=ui/message_box.cpp ui/ui.cpp ui/widget.cpp world.cc

UNAME := $(shell uname)
ifneq (,$(findstring Darwin,$(UNAME)))
	LDFLAGS+=-framework OpenGL
else
	LDFLAGS+=$(shell pkg-config glew --libs)
endif

#OBJS=$(SRCS:.cpp=.o)
SRCS_WITH_PATH=$(addprefix src/,$(SRCS))
BASE_NAMES=$(basename $(SRCS_WITH_PATH))
OBJS=$(addsuffix .o,$(BASE_NAMES))

all: $(TARGET)

.cpp.o: $(SRCS_WITH_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(SRC_DIR)main.cc $(OBJS)
	$(CC) $(CFLAGS) $(SRC_DIR)main.cc $(OBJS) -o $(TARGET) $(LIB_PATH) $(LDFLAGS)

clean:
	echo $(OBJS)
