LIBS=`pkg-config --cflags --libs allegro-5 allegro_acodec-5 allegro_audio-5 allegro_color-5 allegro_dialog-5 allegro_font-5 allegro_image-5 allegro_main-5 allegro_memfile-5 allegro_physfs-5 allegro_primitives-5 allegro_ttf-5` -lm

INCLUDES=-I/usr/include/allegro5/

all: candy


candy: candy.o
	gcc -g -O3 -o candy candy.o $(LIBS)

candy.o: candy.c
	gcc -g -O3 -c candy.c $(LIBS) 

clean:
	rm -f candy.o
	rm -f candy


