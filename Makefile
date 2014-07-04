#Makefile for gestureControl program

MAIN=main.cpp
EXEC=gestureControl
OPENCV=`pkg-config opencv --cflags --libs`
#FILES=gestureDetector.cpp

all:
	g++ $(MAIN) -o $(EXEC) -g -Wall $(OPENCV)

clean:
	rm $(EXEC) 
