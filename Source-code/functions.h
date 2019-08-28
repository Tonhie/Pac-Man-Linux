#include <termios.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>

#define SetPos(x, y) printf("\033[%d;%dH", x, (y << 1) - 1)

#define Cursor_Status(temp) printf("\033[?25%c", temp ? 'h' : 'l')
 // Hide the cursor

struct termios oldt, newt;

inline void Termanal_Prepare(void) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt, newt.c_lflag &= ~(ICANON | ECHO);
}

#define ReadOne_Mode(temp) tcsetattr(STDIN_FILENO, TCSANOW, temp ? &newt : &oldt);


inline bool _kbhit(void) {
    struct timeval tv;
    fd_set rdfs;
    tv.tv_sec = 0, tv.tv_usec = 0;
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);
    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &rdfs);
}