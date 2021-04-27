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
    int road_max_x = road->x - PADDING_X - 1;
    int road_max_y = road->y - PADDING_Y - 1;

    while (loop < LOOPS && !road->stop_flag)
    {
        bool locked = false;

        drive(road_max_x, true, 1.4);
        drive(road_max_y, false, 0.8);
        drive(PADDING_X, true, 1.4);
        drive(PADDING_Y, false, 0.8);

        loop++;
    }
    finished = true;
}

void Car::drive(int max_point, bool axis, float multiplier)
{
    bool locked = false;
    int *current_point;

    if (axis)
        current_point = &this->current_x;
    else
        current_point = &this->current_y;

    if (*current_point < max_point)
    {
        while (*current_point < max_point && !road->stop_flag)
        {
            *current_point += speed * multiplier;

            if (*current_point > max_point)
                *current_point = max_point;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    else if (*current_point > max_point)
    {
        while (*current_point > max_point && !road->stop_flag)
        {
            *current_point -= speed * multiplier;

            if (*current_point < max_point)
                *current_point = max_point;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

bool Car::is_in_blocked_x(int position)
{
    auto blocked_x_start = road->blocked_x_area[position].first;
    auto blocked_x_end = road->blocked_x_area[position].second;

    if (current_x >= blocked_x_start && current_x <= blocked_x_end)
    {
        return true;
    }
    return false;
}

bool Car::is_in_blocked_y(int position)
{
    auto blocked_x_start = road->blocked_y_area[position].first;
    auto blocked_x_end = road->blocked_y_area[position].second;

    if (current_x >= blocked_x_start && current_x <= blocked_x_end)
    {
        return true;
    }
    return false;
}