// Jak samochód dogoni samochód przed nim jadący, to powinien czekać z wyprzedzeniem 
// do momentu aż znajdzie się w obszarze, gdzie jest dopuszczalne wyprzedzanie,
// czyli na każdej stronie / na każdej ścianie ocznaczy Pan pewien odcinek, w którym jest dopuszczalne 
// wyprzedzanie i tylko tam będą się samochody wyprzedzały, a inaczej będą się trzymać jeden za drugim.


#include <ncurses.h>
#include <chrono>

#include "Road.h"
#include "Car.h"

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
        return 0;
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
        this_thread::sleep_for(std::chrono::milliseconds(33));
        road->draw();
        refresh();

        if (kbhit())
        {
            road->stop();
            break;
        }
    }

    endwin();
    return 0;
}