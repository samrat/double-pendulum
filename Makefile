CFLAGS = -Wall -std=c11 --pedantic -Igl3w
LIBS =  -lm -lGL -lglfw -ldl
SRC = pendulum.c gl3w/gl3w.c

pen: $(SRC)
	gcc $(CFLAGS) $(SRC) -o pen $(LIBS)
