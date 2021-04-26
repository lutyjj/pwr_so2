#include <ncurses.h>
#include <random>
#include <mutex>
#include "Car.h"
#include "Road.h"

Car::Car(int number, Road *road)
{
    this->number = number;
    this->road = road;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<> dist(1, 4);
    this->speed = dist(rng);

    t = new thread([this]() { thread_func(); });
}

Car::~Car() {
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

        while (current_x < road_max_x && !road->stop_flag)
        {
            if (current_x > 40 && current_x < road_max_x - 40 && !locked)
            {
                road->mtx.lock();
                locked = true;
            }

            this->current_x = static_cast<int>(x);
            x += speed * 1.4;

            if (current_x > road_max_x - 40 && locked)
            {
                road->mtx.unlock();
                locked = false;
            }

            if (x > road_max_x)
                x = road_max_x;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        while (current_y < road_max_y && !road->stop_flag)
        {
            this->current_y = static_cast<int>(y);
            y += speed * 0.8;

            if (y > road_max_y)
                y = road_max_y;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        while (current_x > PADDING_X && !road->stop_flag)
        {
            this->current_x = static_cast<int>(x);
            x -= speed * 1.4;

            if (x < PADDING_X)
                x = PADDING_X;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        while (current_y > PADDING_Y && !road->stop_flag)
        {
            this->current_y = static_cast<int>(y);
            y -= speed * 0.8;

            if (y < PADDING_Y)
                y = PADDING_Y;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        loop++;
    }
    finished = true;
}