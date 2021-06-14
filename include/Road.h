#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>


using namespace std;

class Car;

enum class Axis;

class Road {
private:
  vector<Car *> cars;
  thread *t_spawn_car;
  thread *t_allowed_road_watcher;

  void spawn_car();
  void watch_segments();
  static void draw_rectangle(int y1, int x1, int y2, int x2);
  static void draw_green_rectangle(int x1, int x2, int y1, int y2);

public:
  Road(int x, int y);
  ~Road();


  vector<queue<Car*>> qx;
  vector<queue<Car*>> qy;

  condition_variable cvx0;
  condition_variable cvx1;
  condition_variable cvy0;
  condition_variable cvy1;

  vector<pair<int, int>> allowed_x;
  vector<pair<int, int>> allowed_y;

  vector<int> allowed_car_amount_x;
  vector<int> allowed_car_amount_y;

  atomic_bool stop_flag{};
  mutex mtx;
  int x = 0;
  int y = 0;

  void notify_add_x(Car *car, int position);
  void notify_remove_x(Car *car, int position);

  void notify_add_y(Car *car, int position);
  void notify_remove_y(Car *car, int position);

  bool is_blocked(Axis axis);
  int cars_on_segment(Axis axis);

  void add_to_queue(Car* car, Axis axis);
  Car* last_car_in_queue(Axis axis);

  vector<vector<Car*>> cars_in_allowed_x;
  vector<vector<Car*>> cars_in_allowed_y;

  vector<bool> blocked_segments_x;
  vector<bool> blocked_segments_y;

  void draw();
  void stop();
  Car *find_nearest_car(Car *car, Axis axis);
};
