#include <ncurses.h>
#include <random>
#include <vector>
#include "Road.h"
#include "Car.h"

Road::Road(int x, int y) {
    this->x = x;
    this->y = y;
    this->stop_flag = false;
    //this->allowed_x.push_back(make_pair(x / 4, x - x / 4));
    this->allowed_x.emplace_back(0, 0);

    this->allowed_x.emplace_back(0, 0);
    //this->allowed_x.emplace_back(x / 5, x / 2);
    this->allowed_y.emplace_back(y / 4, y / 2);
    this->allowed_y.emplace_back(y / 3, y - y / 5);

    t_spawn_car = new thread([this]() { spawn_car(); });
}

Road::~Road() {
    clear();
    addstr("Waiting for threads...\n");
    refresh();

    t_spawn_car->join();

    for (auto car : cars)
        delete car;

    cars.clear();
}

void Road::draw() {
    draw_rectangle(0, 0, y - 1, x - 1);
    draw_rectangle(2, 4, y - 3, x - 5);

    draw_green_rectangle(allowed_x[0].first + 1, allowed_x[0].second + 2, 1, 2);
    draw_green_rectangle(x - 4, x - 1, allowed_y[0].first, allowed_y[0].second);
    draw_green_rectangle(allowed_x[1].first + 1, allowed_x[1].second + 2, y - 2, y - 1);
    draw_green_rectangle(1, 4, allowed_y[1].first, allowed_y[1].second);

    mtx.lock();
    for (int i = 0; i < cars.size(); i++) {
        auto car = cars[i];

        if (car->finished)
            cars.erase(cars.begin() + i);
        else
            mvprintw(car->current_y, car->current_x, "%d", car->number);
    }
    mtx.unlock();
}

void Road::draw_rectangle(int y1, int x1, int y2, int x2) {
    mvhline(y1, x1, 0, x2 - x1);
    mvhline(y2, x1, 0, x2 - x1);
    mvvline(y1, x1, 0, y2 - y1);
    mvvline(y1, x2, 0, y2 - y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

void Road::draw_green_rectangle(int x1, int x2, int y1, int y2) {
    for (int i = x1; i < x2; i++) {
        for (int j = y1; j < y2; j++) {
            mvaddch(j, i, ' ' | COLOR_PAIR(1));
        }
    }
}

void Road::spawn_car() {
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<> dist(500, 3000);

    int count = 0;
    while (!this->stop_flag && cars.size() < 10) //  && cars.size() < 10
    {
        count++;
        cars.push_back(new Car(count, this));
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
    }
}

Car *Road::find_nearest_car(Car *param_car, bool is_moving_forward, bool is_x_axis) {
    Car *nearest_car = nullptr;
    float prev_nearest_max = INT64_MAX;
    float prev_nearest_min = -1;

    mtx.lock();
    
    if (is_x_axis) {
        for (auto &car : cars) {
            if (car->current_x == param_car->current_x)
                continue;

            if (is_moving_forward) {
                if (car->current_x > param_car->current_x
                    && car->current_x < prev_nearest_max
                    && car->current_y == param_car->current_y) {
                    prev_nearest_max = car->current_x;
                    nearest_car = car;
                }
            } else {
                if (car->current_x < param_car->current_x
                    && car->current_x > prev_nearest_min 
                    && car->current_y == param_car->current_y) {
                    prev_nearest_min = car->current_x;
                    nearest_car = car;
                }
            }
        }
    } else {
        for (auto &car : cars) {
            if (car->current_y == param_car->current_y)
                continue;

            if (is_moving_forward) {
                if (car->current_y > param_car->current_y
                    && car->current_y < prev_nearest_max
                    && car->current_x == param_car->current_x) {
                    prev_nearest_max = car->current_y;
                    nearest_car = car;
                }
            } else {
                if (car->current_y < param_car->current_y
                    && car->current_y > prev_nearest_min 
                    && car->current_x == param_car->current_x) {
                    prev_nearest_min = car->current_y;
                    nearest_car = car;
                }
            }
        }
    }

    mtx.unlock();

    return nearest_car;
}

void Road::stop() {
    stop_flag = true;
    mtx.unlock();

    delete this;
}
