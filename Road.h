#pragma once
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

struct Car;

struct Road {
    int x = 0;
    int y = 0;
    atomic_bool stop_flag;
    vector<Car *> cars;
    thread* t_spawn_car;
    mutex mtx;

    Road(int x, int y);
    ~Road();
    void draw();

private:
    void spawn_car();
    void rectangle(int y1, int x1, int y2, int x2);
};