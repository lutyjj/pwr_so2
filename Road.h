#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <list>

using namespace std;

class Car;

class Road {
private:
    vector<Car *> cars;
    thread *t_spawn_car;

    void spawn_car();
    static void draw_rectangle(int y1, int x1, int y2, int x2);
    static void draw_green_rectangle(int x1, int x2, int y1, int y2);

public:
    Road(int x, int y);
    ~Road();

    vector<pair<int, int>> allowed_x;
    vector<pair<int, int>> allowed_y;

    atomic_bool stop_flag{};
    mutex mtx;
    int x = 0;
    int y = 0;

    void draw();
    void stop();
    Car *find_nearest_car(Car *car, bool is_moving_forward, bool is_x_axis);
};
