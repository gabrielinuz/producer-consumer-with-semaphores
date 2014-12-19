.PHONY: all clean

all:
	g++ ./src/main.cpp -o producer-consumer-with-semaphores -lpthread
	
clean:
	rm producer-consumer-with-semaphores