# Compile

## WINDOWS:
    Expects ncurses.h
    gcc "./qchin.c" -o "./qchin.exe" -I CURSES_INCLUDE_PATH CURSES_LIB_PATH

## LINUX
    Expects ncurses/curses.h
	gcc "./qchin.c" -o "./qchin" -I CURSES_INCLUDE_PATH -L CURSES_LIB_PATH -lncurses -DNCURSES_STATIC

## Makefile
    Replace line 5 with one of the commands above then run:
    make build