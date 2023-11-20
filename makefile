CC=gcc
CFLAGS=$(shell pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 --cflags)
LIBS=$(shell pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 --libs) -lallegro -lallegro_image -lallegro_ttf

SRCS= space.c cheat.c enemy.c Joystick.c obstaculo.c PowerUp.c ship.c shot.c
OBJS=$(SRCS:.c=.o)

TARGET= Space_Invaders

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
