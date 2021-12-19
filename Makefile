build:
	gcc consumer.c -o consumer.out

clean:
	rm -f *.out

all: clean build

run:
	./consumer.out