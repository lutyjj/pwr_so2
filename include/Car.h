#pragma once

#include <mutex>
#include <thread>

#define PADDING_X 2
#define PADDING_Y 1
#define LOOPS 3

using namespace std;

class Road;

enum class Axis { x_positive, x_negative, y_positive, y_negative };

class Car {
private:
  int loop = 0;
  float speed = 0;
  float base_speed = 2;

  Road *road;
  Axis axis;
  thread *t;
  mutex mtx;

  void thread_func();
  void drive(int max_x, Axis axis, float mult_x);
  bool is_in_allowed_x(int position);
  bool is_in_allowed_y(int position);
  float nearest_car_speed(Axis axis);
  Car *nearest_car(Axis axis);
  void check_for_sleep(Axis axis, int current_point);

public:
  Car(int number, Road *road);
  ~Car();

  float current_x = 0 + PADDING_X;
  float current_y = 0 + PADDING_Y;
  int number = 0;
  bool finished = false;

  bool check_for_remove_x = false;
  bool check_for_remove_y = false;

  bool is_near_start(Axis axis, float current_point, int overhead = 5);
};
