#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>
#include <random>

using namespace std;

struct Car
{
    int current_x = 0;
    int current_y = 0;
    float speed = 0;
    int number;
    int loop = 0;
    thread *t;

    Car(int number)
    {
        this->number = number;

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<> dist(1, 4);
        this->speed = dist(rng);

        t = new thread([this]() { thread_func(); });
    }

    void thread_func()
    {
        float x = 0;
        float y = 0;

        while (loop < 1)
        {
            while (current_x < 100)
            {
                this->current_x = static_cast<int>(x);
                x += speed * 1.4;

                if (x > 100)
                    x = 100;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            while (current_y < 10)
            {
                this->current_y = static_cast<int>(y);
                y += speed * 0.8;

                if (y > 10)
                    y = 10;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            while (current_x > 0)
            {
                this->current_x = static_cast<int>(x);
                x -= speed * 1.4;

                if (x < 0)
                    x = 0;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            while (current_y > 0)
            {
                this->current_y = static_cast<int>(y);
                y -= speed * 0.8;

                if (y < 0)
                    y = 0;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            loop++;
        }

        t->join();
    }
};

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
    initscr();
    nodelay(stdscr, TRUE);

    vector<Car *> cars;

    for (int i = 0; i < 3; i++)
    {
        cars.push_back(new Car(i));
    }

    while (true)
    {
        clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

        for (auto car : cars)
        {
            mvprintw(car->current_y, car->current_x, "%d", car->number);
        }
        refresh();

        if (kbhit())
            break;
    }

    endwin();

    for (auto &car : cars)
    {
        delete car;
    }
    cars.clear();

    return 0;
}