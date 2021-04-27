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
    this->allowed_x.push_back(make_pair(40, 60));
    this->allowed_x.push_back(make_pair(20, 70));
    this->allowed_y.push_back(make_pair(4, 8));
    this->allowed_y.push_back(make_pair(2, 10));

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

void draw_green_rectangle(int x1, int x2, int y1, int y2)
{
    for (int i = x1; i < x2; i++)
    {
        for (int j = y1; j < y2; j++)
        {
            mvaddch(j, i, ' ' | COLOR_PAIR(1));
        }
    }
}

void Road::draw()
{
    draw_rectangle(0, 0, y - 1, x - 1);
    draw_rectangle(2, 4, y - 3, x - 5);

    draw_green_rectangle(allowed_x[0].first + 1, allowed_x[0].second + 2, 1, 2);
    draw_green_rectangle(x - 4, x - 1, allowed_y[0].first, allowed_y[0].second);
    draw_green_rectangle(allowed_x[1].first + 1, allowed_x[1].second + 2, y - 2, y - 1);
    draw_green_rectangle(1, 4, allowed_y[1].first, allowed_y[1].second);

    for (int i = 0; i < cars.size(); i++)
    {
        auto car = cars[i];

        if (car->finished)
            cars.erase(cars.begin() + i);
        else
            mvprintw(car->current_y, car->current_x, "%d", car->number);
    }
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
    while (!this->stop_flag) //  && cars.size() < 10
    {
        count++;
        cars.push_back(new Car(count, this));
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
    }
}

Car* Road::find_nearest_car(float x, bool is_moving_forward, bool is_x_axis)
{
    Car* nearest_car = nullptr;
    float prev_nearest = 0;

    mtx.lock();

    if (is_moving_forward && is_x_axis)
    {
        for (auto &car : cars)
        {
            if (car->current_x == x)
                continue;

            if (prev_nearest == 0) {
                prev_nearest = car->current_x;
            }

            if (car->current_x > x && prev_nearest >= car->current_x) {
                prev_nearest = car->current_x;
                nearest_car = car;
            }
        }
    }
    mtx.unlock();

    return nearest_car;
}

void Road::stop()
{
    stop_flag = true;
    mtx.unlock();

    delete this;
}