#include <ncurses.h>
#include <random>
#include <mutex>
#include "Car.h"
#include "Road.h"

Car::Car(int number, Road *road)
{
    this->number = number;
    this->road = road;

    random_device rd;
    mt19937 rng(rd());
    uniform_real_distribution<> dist(1, 4);
    this->speed = dist(rng);

    t = new thread([this]() { thread_func(); });
}

Car::~Car()
{
    t->join();

    printw("Thread %d exited successfully.\n", number);
    refresh();
}

void Car::thread_func()
{
    float x = 0 + PADDING_X;
    float y = 0 + PADDING_Y;

    int road_max_x = road->x - PADDING_X - 1;
    int road_max_y = road->y - PADDING_Y - 1;

    while (loop < LOOPS && !road->stop_flag)
    {
        bool locked = false;

        drive(&x, road_max_x, true, 1.4);
        drive(&y, road_max_y, false, 0.8);
        drive(&x, PADDING_X, true, 1.4);
        drive(&y, PADDING_Y, false, 0.8);

        loop++;
    }
    finished = true;
}

void Car::drive(float *x, int max_x, bool axis, float mult_x) {
    bool locked = false;
    int* current_point;

    if (axis)
        current_point = &this->current_x;
    else
        current_point = &this->current_y;

    if (*x < max_x) {
        while (*current_point < max_x && !road->stop_flag)
        {
            *current_point = static_cast<int>(*x);
            *x += speed * mult_x;

            if (*x > max_x) *x = max_x;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    else if (*x > max_x) {
        while (*current_point > max_x && !road->stop_flag)
        {
            *current_point = static_cast<int>(*x);
            *x -= speed * mult_x;

            if (*x < max_x) *x = max_x;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
}

bool Car::is_in_blocked_top_x()
{
    auto blocked_x_start = road->blocked_x_area[0].first;
    auto blocked_x_end = road->blocked_x_area[0].second;

    if (current_x >= blocked_x_start && current_x <= blocked_x_end) {
        return true;
    }
    return false;
}

bool Car::is_in_blocked_bottom_x()
{
    auto blocked_x_start = road->blocked_x_area[1].first;
    auto blocked_x_end = road->blocked_x_area[1].second;

    if (current_x >= blocked_x_start && current_x <= blocked_x_end) {
        return true;
    }
    return false;
}