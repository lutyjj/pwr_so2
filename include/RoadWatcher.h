#pragma once
#include <thread>
#include <list>
#include <vector>

using namespace std;

struct Car;
struct Road;
enum class Axis;

struct RoadWatcher
{
    int start, end;
    Axis axis;

    vector<Car*> car_query;
    Road *road;
    
    void notify_add(Car *car);
    void notify_remove(Car *car);

    RoadWatcher(int start, int end, Road *road);
    ~RoadWatcher();
};
