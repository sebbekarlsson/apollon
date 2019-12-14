exec = apollon.out
sources = $(wildcard src/*.c)
sources += $(wildcard ../GL/src/*.c)
objects = $(sources:.c=.o)
flags = -Wall -g `pkg-config --cflags coelum` -lathena -lhermes -I../coelum/GL/include -lm -lpng -lsqlite3 `pkg-config --libs coelum`


$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

clean:
	-rm *.out
	-rm *.o
	-rm src/*.o
