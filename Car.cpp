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

        drive_forward(road_max_x, true, 1.4);
        drive_forward(road_max_y, false, 0.8);
        drive_backward(PADDING_X, true, 1.4);
        drive_backward(PADDING_Y, false, 0.8);

        loop++;
    }
    finished = true;
}

void Car::drive_forward(int max_point, bool axis, float multiplier)
{
    bool locked = false;
    float prev_speed = speed;
    float *current_point;

    if (axis)
        current_point = &this->current_x;
    else
        current_point = &this->current_y;

    while (*current_point < max_point && !road->stop_flag)
    {
        if (max_point != 16 && max_point != PADDING_X && max_point && PADDING_Y)
        {
            if (is_in_blocked_x(0))
            {
                this->speed = lookahead();
            }
            else
            {
                this->speed = prev_speed;
            }
            // if (is_in_blocked_x(0) && !locked)
            // {
            //     auto temp = lookahead();
            //     //this->speed = temp;
            //     locked = true;
            // }

            // if (!is_in_blocked_x(0) && locked)
            // {
            //     locked = false;
            //     this->speed = prev_speed;
            // }
        }

        *current_point += speed * multiplier;

        if (*current_point > max_point)
            *current_point = max_point;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Car::drive_backward(int min_point, bool axis, float multiplier)
{
    bool locked = false;
    float prev_speed = speed;
    float *current_point;

    if (axis)
        current_point = &this->current_x;
    else
        current_point = &this->current_y;

    while (*current_point > min_point && !road->stop_flag)
    {
        *current_point -= speed * multiplier;

        if (*current_point < min_point)
            *current_point = min_point;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

float Car::lookahead()
{
    road->mtx.lock();
    auto nearest_car_speed = road->find_nearest_car(current_x);
    if (nearest_car_speed == -1 || nearest_car_speed >= this->speed)
    {
        road->mtx.unlock();
        return this->speed;
    }
    road->mtx.unlock();
    return nearest_car_speed;
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