// Jak samochód dogoni samochód przed nim jadący, to powinien czekać z wyprzedzeniem 
// do momentu aż znajdzie się w obszarze, gdzie jest dopuszczalne wyprzedzanie,
// czyli na każdej stronie / na każdej ścianie ocznaczy Pan pewien odcinek, w którym jest dopuszczalne 
// wyprzedzanie i tylko tam będą się samochody wyprzedzały, a inaczej będą się trzymać jeden za drugim.


#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <atomic>
#include <mutex>

#define LOOPS 3
#define MAX_CARS 4
#define PADDING_X 2
#define PADDING_Y 1

using namespace std;
atomic_bool stop_flag;
mutex mtx;
void rectangle(int y1, int x1, int y2, int x2);

struct Road {
    int x = 0;
    int y = 0;



    Road(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void draw() {
        rectangle(0, 0, y - 1, x - 1);
        rectangle(2, 4, y - 3, x - 5);
    }
};

struct Car
{
    int current_x = 0 + PADDING_X;
    int current_y = 0 + PADDING_Y;
    float speed = 0;
    int number;
    int loop = 0;
    bool finished = false;
    thread *t;
    Road* road;

    Car(int number, Road* road)
    {
        this->number = number;
        this->road = road;
        
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

        int road_max_x = road->x - PADDING_X - 1;
        int road_max_y = road->y - PADDING_Y - 1;

        while (loop < LOOPS && !stop_flag)
        {
            bool locked = false;

            while (current_x < road_max_x && !stop_flag)
            {
                if (current_x > 40 && current_x < road_max_x - 40 && !locked) {
                    mtx.lock();
                    locked = true;
                }

                this->current_x = static_cast<int>(x);
                x += speed * 1.4;

                if (current_x > road_max_x - 40 && locked) {
                    mtx.unlock();
                    locked = false;
                }

                if (x > road_max_x)
                    x = road_max_x;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            while (current_y < road_max_y && !stop_flag)
            {
                this->current_y = static_cast<int>(y);
                y += speed * 0.8;

                if (y > road_max_y)
                    y = road_max_y;

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

void spawn_car(vector<Car *> &cars, Road* road)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> dist(500, 3000);

    int count = 0;
    while (!stop_flag && cars.size() < MAX_CARS)
    {
        count++;
        cars.push_back(new Car(count, road));
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

    Road* road = new Road(COLS, LINES);
    vector<Car *> cars;
    thread t_spawn_car(spawn_car, ref(cars), ref(road));


    while (true)
    {
        clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

        //box(win, 0, 0);
        road->draw();

        for (auto car : cars)
        {
            if (!car->finished)
                mvprintw(car->current_y, car->current_x, "%d", car->number);
        }
        refresh();

        if (kbhit())
        {
            stop_flag = true;
            mtx.unlock();
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