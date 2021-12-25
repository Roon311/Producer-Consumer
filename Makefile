build:
	gcc consumer.c -o consumer.out
	gcc Producer.c -o producer.out

clean:
	rm -f *.out



all: clean build

run:
	./producer.out

