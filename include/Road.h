#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include "RoadWatcher.h"

using namespace std;

class Car;

enum class Axis;

class Road {
private:
  thread *t_spawn_car;

  void spawn_car();
  static void draw_rectangle(int y1, int x1, int y2, int x2);
  static void draw_green_rectangle(int x1, int x2, int y1, int y2);

public:
  Road(int x, int y);
  ~Road();

  vector<Car *> cars;
  vector<pair<int, int>> allowed_x;
  vector<pair<int, int>> allowed_y;
  RoadWatcher* road_watcher;

  atomic_bool stop_flag{};
  mutex mtx;
  int x = 0;
  int y = 0;

  void notify_add(int number);
  void notify_remove(int number);
  void notify_add(Car *car);
  void notify_remove(Car *car);
  
  vector<int> in_allowed_x;

  void draw();
  void stop();
  Car *find_nearest_car(Car *car, Axis axis);
};
