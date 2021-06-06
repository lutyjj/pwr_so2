#include "RoadWatcher.h"
#include "Road.h"
#include <algorithm>
#include "Car.h"

using namespace std;

RoadWatcher::RoadWatcher(int start, int end, Road *road) {
    this->start = start;
    this->end = end;

  this->road = road;

}