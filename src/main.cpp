// Jak samochód dogoni samochód przed nim jadący, to powinien czekać z
// wyprzedzeniem do momentu aż znajdzie się w obszarze, gdzie jest dopuszczalne
// wyprzedzanie, czyli na każdej stronie / na każdej ścianie ocznaczony jest
// pewien odcinek, w którym jest dopuszczalne wyprzedzanie i tylko tam będą się
// samochody wyprzedzały, a inaczej będą się trzymać jeden za drugim.

// Jak na wjeździe do tych sekcji, gdzie jest wyprzedzanie możliwe, 
// będzie więcej niż 3 czy 4 samochody (specyficzne dla każdej sekcji),
// to samochody będą się ustawiały w kolejce jeden za drugim. 
// I w momencie jak samochod będzie wyjeżdżał z tej sekcji krytycznej,
// to zostanie wpuszczony następny pojazd z kolei.

#include <chrono>
#include <iostream>
#include <ncurses.h>

#include "Car.h"
#include "Road.h"

using namespace std;

int kbhit() {
  int ch = getch();

  if (ch != ERR) {
    return 1;
  } else
    return 0;
}

int main() {
  WINDOW *win = initscr();
  nodelay(stdscr, TRUE);
  curs_set(0);
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_GREEN);
  init_pair(2, COLOR_WHITE, COLOR_BLACK);
  Road *road = new Road(COLS, LINES);

  while (true) {
    erase();
    this_thread::sleep_for(std::chrono::microseconds(16666));
    road->draw();
    refresh();

    if (kbhit()) {
      road->stop();
      break;
    }
  }

  endwin();
  return 0;
}
