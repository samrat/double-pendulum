CFLAGS = -Wall -std=c11 --pedantic -Igl3w
LIBS =  -lm -lGL -lglfw -ldl

pen: pendulum.c gl3w/gl3w.c
	gcc $(CFLAGS) pendulum.c -o pen $(LIBS)
