#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <atomic>

#define LOOPS 1

using namespace std;
atomic_bool stop_flag;

struct Car
{
    int current_x = 0;
    int current_y = 0;
    float speed = 0;
    int number;
    int loop = 0;
    bool finished = false;
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

        int cols = COLS - 1;
        int lines = LINES - 1;

        while (loop < LOOPS)
        {
            while (current_x < cols)
            {
                this->current_x = static_cast<int>(x);
                x += speed * 1.4;

                if (x > cols)
                    x = cols;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            while (current_y < lines)
            {
                this->current_y = static_cast<int>(y);
                y += speed * 0.8;

                if (y > lines)
                    y = lines;

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
        finished = true;
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

void spawn_car(vector<Car *> &cars) {
    int count = 0;
    while (!stop_flag)
    {
        count++;
        cars.push_back(new Car(count));
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}

int main()
{
    stop_flag = false;

    WINDOW* win = initscr();
    nodelay(stdscr, TRUE);
    curs_set(0);

    vector<Car *> cars;

    // for (int i = 0; i < 3; i++)
    // {
    //     cars.push_back(new Car(i));
    // }

    thread t_spawn_car(spawn_car, ref(cars));

    while (true)
    {
        clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

            box(win, 0, 0);

        for (auto car : cars)
        {
            if (!car->finished)
                mvprintw(car->current_y, car->current_x, "%d", car->number);
        }
        refresh();

        if (kbhit()) {
            stop_flag = true;
            break;
        }
    }

    clear();
    addstr("Waiting for threads...\n");
    refresh();

    t_spawn_car.join();

    for (auto &car : cars)
    {
        car->t->join();
        printw("Thread %d exited successfully.\n", car->number);
        delete car;
        refresh();
    }
    cars.clear();

    endwin();
    return 0;
}