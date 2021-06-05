#include "RoadWatcher.h"
#include "Road.h"
#include <algorithm>
#include "Car.h"

using namespace std;

RoadWatcher::RoadWatcher(int start, int end, Road *road) {
    this->start = start;
    this->end = end;

  this->road = road;

    t_car_watcher = new thread([this]() { watch(); });
}

void RoadWatcher::watch() {
    // while (!road->stop_flag)
    // {
    //     for (Car *car : road->cars) {
    //         if (car->speed && car->current_x > start && car->current_x < end)
    //             query(car);
    //         else
    //             remove(car);
    //     }
    // }
}

void RoadWatcher::query(Car* car) {
    auto index = find(car_query.begin(), car_query.end(), car);

    if (index == car_query.end()) {
        car_query.push_back(car);
    }
}

void RoadWatcher::remove(Car* car) {
    // auto index = find(car_query.begin(), car_query.end(), car);

    // if (index != car_query.end()) {
    //     car_query.erase(index);
    // }5`

    car_query.erase(std::remove(car_query.begin(), car_query.end(), car), car_query.end());
}