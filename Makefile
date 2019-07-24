exec = apollon.out
sources = $(wildcard src/*.c)
sources += $(wildcard ../GL/src/*.c)
objects = $(sources:.c=.o)
flags = -Wall -g -lathena -lspr -lcoelum -lhermes -I../coelum/GL/include -lglfw -ldl -lcglm -lm -lopenal -lpthread -laudio -lpng -lsqlite3


$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

clean:
	-rm *.out
	-rm *.o
	-rm src/*.o
