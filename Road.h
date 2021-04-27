#pragma once
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <list>

using namespace std;

class Car;

class Road
{
private:
    vector<Car *> cars;
    thread *t_spawn_car;
    void spawn_car();
    void draw_rectangle(int y1, int x1, int y2, int x2);

public:
    Road(int x, int y);
    ~Road();

    vector<pair<int, int>> blocked_x_area;
    vector<pair<int, int>> blocked_y_area;

    atomic_bool stop_flag;
    mutex mtx;
    int x = 0;
    int y = 0;

    void draw();
    void stop();
};