// Jak samochód dogoni samochód przed nim jadący, to powinien czekać z wyprzedzeniem 
// do momentu aż znajdzie się w obszarze, gdzie jest dopuszczalne wyprzedzanie,
// czyli na każdej stronie / na każdej ścianie ocznaczy Pan pewien odcinek, w którym jest dopuszczalne 
// wyprzedzanie i tylko tam będą się samochody wyprzedzały, a inaczej będą się trzymać jeden za drugim.


#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <atomic>
#include <mutex>

#include "Road.h"
#include "Car.h"


#define MAX_CARS 6


using namespace std;

int kbhit(void)
{
    int ch = getch();

    if (ch != ERR)
    {
        ungetch(ch);
        return 1;
    }
    else
    {
        return 0;
    }
}

int main()
{
    WINDOW *win = initscr();
    nodelay(stdscr, TRUE);
    curs_set(0);

    Road* road = new Road(COLS, LINES);

    while (true)
    {
        clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

        //box(win, 0, 0);
        road->draw();

        for (auto car : road->cars)
        {
            if (!car->finished)
                mvprintw(car->current_y, car->current_x, "%d", car->number);
        }
        refresh();

        if (kbhit())
        {
            road->stop_flag = true;
            road->mtx.unlock();
            break;
        }
    }

    clear();
    addstr("Waiting for threads...\n");
    refresh();

    for (auto car : road->cars)
    {
        car->t->join();
        printw("Thread %d exited successfully.\n", car->number);
        delete car;
        refresh();
    }
    road->cars.clear();

    endwin();
    return 0;
}