#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <atomic>

#define LOOPS 3
#define PADDING_X 2
#define PADDING_Y 1

using namespace std;
atomic_bool stop_flag;

struct Car
{
    int current_x = 0 + PADDING_X;
    int current_y = 0 + PADDING_Y;
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
        float x = 0 + PADDING_X;
        float y = 0 + PADDING_Y;

        int cols = COLS - PADDING_X - 1;
        int lines = LINES - PADDING_Y - 1;

        while (loop < LOOPS && !stop_flag)
        {
            while (current_x < cols && !stop_flag)
            {
                this->current_x = static_cast<int>(x);
                x += speed * 1.4;

                if (x > cols)
                    x = cols;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            while (current_y < lines && !stop_flag)
            {
                this->current_y = static_cast<int>(y);
                y += speed * 0.8;

                if (y > lines)
                    y = lines;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            while (current_x > PADDING_X && !stop_flag)
            {
                this->current_x = static_cast<int>(x);
                x -= speed * 1.4;

                if (x < PADDING_X)
                    x = PADDING_X;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            while (current_y > PADDING_Y && !stop_flag)
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

void spawn_car(vector<Car *> &cars)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> dist(500, 3000);

    int count = 0;
    while (!stop_flag)
    {
        count++;
        cars.push_back(new Car(count));
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
    }
}

void rectangle(int y1, int x1, int y2, int x2)
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

int main()
{
    stop_flag = false;

    WINDOW *win = initscr();
    nodelay(stdscr, TRUE);
    curs_set(0);

    vector<Car *> cars;
    thread t_spawn_car(spawn_car, ref(cars));

    while (true)
    {
        clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

        //box(win, 0, 0);
        rectangle(0, 0, LINES - 1, COLS - 1);
        rectangle(2, 4, LINES - 3, COLS - 5);

        for (auto car : cars)
        {
            if (!car->finished)
                mvprintw(car->current_y, car->current_x, "%d", car->number);
        }
        refresh();

        if (kbhit())
        {
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