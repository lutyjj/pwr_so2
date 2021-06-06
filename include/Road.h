#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

class Car;

enum class Axis;

class Road {
private:
  thread *t_spawn_car;
  thread *t_allowed_road_watcher;

  void spawn_car();
  void watch_segments();
  static void draw_rectangle(int y1, int x1, int y2, int x2);
  static void draw_green_rectangle(int x1, int x2, int y1, int y2);

public:
  Road(int x, int y);
  ~Road();

  vector<Car *> cars;
  vector<pair<int, int>> allowed_x;
  vector<pair<int, int>> allowed_y;

  atomic_bool stop_flag{};
  mutex mtx;
  int x = 0;
  int y = 0;

  void notify_add_x(Car *car, int position);
  void notify_remove_x(Car *car, int position);

  void notify_add_y(Car *car, int position);
  void notify_remove_y(Car *car, int position);
  
  bool is_blocked_x(int position);
  bool is_blocked_y(int position);

  vector<vector<int>> in_allowed_x;
  vector<vector<int>> in_allowed_y;

  vector<bool> blocked_x;
  vector<bool> blocked_y;

  void draw();
  void stop();
  Car *find_nearest_car(Car *car, Axis axis);
};
