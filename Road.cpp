#include <ncurses.h>
#include <random>
#include <vector>
#include "Road.h"
#include "Car.h"

Road::Road(int x, int y)
{
    this->x = x;
    this->y = y;
    this->stop_flag = false;

    t_spawn_car = new thread([this]() { spawn_car(); });
}

Road::~Road()
{
    clear();
    addstr("Waiting for threads...\n");
    refresh();

    t_spawn_car->join();

    for (auto car : cars)
        delete car;

    cars.clear();
}

void Road::draw()
{
    draw_rectangle(0, 0, y - 1, x - 1);
    draw_rectangle(2, 4, y - 3, x - 5);

    for (int i = 0; i < cars.size(); i++) {
        auto car = cars[i];

        if (car->finished)
            cars.erase(cars.begin() + i);
        else
            mvprintw(car->current_y, car->current_x, "%d", car->number);
    }

    // for (auto car : cars)
    // {
    //     if (!car->finished)
    //         mvprintw(car->current_y, car->current_x, "%d", car->number);
    //     else {
    //         cars.erase(car);
    //     }
    // }
}

void Road::draw_rectangle(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2 - x1);
    mvhline(y2, x1, 0, x2 - x1);
    mvvline(y1, x1, 0, y2 - y1);
    mvvline(y1, x2, 0, y2 - y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

void Road::spawn_car()
{
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<> dist(500, 3000);

    int count = 0;
    while (!this->stop_flag && cars.size() < 5)
    {
        count++;
        cars.push_back(new Car(count, this));
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
    }
}

void Road::stop() {
    stop_flag = true;
    mtx.unlock();


    delete this;
}