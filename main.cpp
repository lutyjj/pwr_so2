#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>

struct Car {
    int pos_x = 0;
    int pos_y = 0;
    int speed = 0;
};

int main() {
    initscr();			/* Start curses mode 		  */
	
    for (int i = 0; i < LINES; i++) {
        clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        mvaddch(0, i, '.');
        refresh();
    }

    getch();
	endwin();			/* End curses mode		  */

    return 0;
}