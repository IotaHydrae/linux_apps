CC=gcc

all: producer consumer

producer: producer.c
	$(CC) producer.c -o producer -pthread

consumer: consumer.c
	$(CC) consumer.c -o consumer -pthread

clean:
	rm -f producer consumer
