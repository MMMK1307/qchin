all: build run

build: qchin.c
	mkdir bin	
	gcc "./qchin.c" -o "./bin/qchin" -I /ucrt64/include -L /ucrt64/lib -lncurses -DNCURSES_STATIC -pedantic -Wall

run:
	"./bin/qchin"