#pragma once
#include <thread>
#include <list>

using namespace std;

struct Car;
struct Road;

struct RoadWatcher
{
    thread *t_car_watcher;
    list<Car*> car_query;
    void query(Car* car);
    void watch();
    int start, end;
    Road *road;

    void remove(Car* car);

    RoadWatcher(int start, int end, Road *road);
    ~RoadWatcher();
};
