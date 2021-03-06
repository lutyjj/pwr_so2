#include "Car.h"
#include "Road.h"

#include <ncurses.h>
#include <random>

Car::Car(int number, Road *road) {
  this->number = number;
  this->road = road;
  this->axis = Axis::x_positive;

  random_device rd;
  mt19937 rng(rd());
  uniform_real_distribution<> dist(1, 2);
  this->speed = dist(rng);

  t = new thread([this]() { thread_func(); });
}

Car::~Car() {
  t->join();

  printw("Thread %d exited successfully.\n", number);
  refresh();
}

void Car::thread_func() {
  int road_max_x = road->x - PADDING_X - 1;
  int road_max_y = road->y - PADDING_Y - 1;

  while (loop < LOOPS && !road->stop_flag) {
    drive(road_max_x, Axis::x_positive, 1.4);
    drive(road_max_y, Axis::y_positive, 0.8);
    drive(PADDING_X, Axis::x_negative, 1.4);
    drive(PADDING_Y, Axis::y_negative, 0.8);
    loop++;
  }

  finished = true;
}

void Car::drive(int end_point, Axis axis, float multiplier) {
  float temp_speed;
  float *current_point;

  if (axis == Axis::x_positive || axis == Axis::x_negative)
    current_point = &this->current_x;
  else
    current_point = &this->current_y;

  switch (axis) {
  case Axis::x_positive:
  case Axis::y_positive:
    while (*current_point < end_point && !road->stop_flag) {
      if (is_in_allowed_x(0) || is_in_allowed_y(0)) {
        base_speed = speed;
      } else {
        check_for_sleep(axis, *current_point);

        float found_speed = nearest_car_speed(axis);

        base_speed = found_speed < speed ? found_speed : speed;
      }

      *current_point += base_speed * multiplier;

      if (*current_point > end_point)
        *current_point = end_point;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    break;

  case Axis::x_negative:
  case Axis::y_negative:
    while (*current_point > end_point && !road->stop_flag) {
      if (is_in_allowed_x(1) || is_in_allowed_y(1)) {
        base_speed = speed;
      } else {
        check_for_sleep(axis, *current_point);

        float found_speed = nearest_car_speed(axis);

        base_speed = found_speed < speed ? found_speed : speed;
      }

      *current_point -= base_speed * multiplier;

      if (*current_point < end_point)
        *current_point = end_point;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    break;

  default:
    break;
  }
}

float Car::nearest_car_speed(Axis axis) {
  auto found_car = road->find_nearest_car(this, axis);

  if (found_car) {
    if (!((axis == Axis::x_negative || axis == Axis::x_positive) &&
          abs(found_car->current_x - current_x) > 8) &&
        !((axis == Axis::y_negative || axis == Axis::y_positive) &&
          abs(found_car->current_y - current_y) > 3))
      return found_car->base_speed;
  }

  return this->base_speed;
}

bool Car::is_in_allowed_x(int position) {
  if (road->allowed_x.size()) {
    auto allowed_x_start = road->allowed_x[position].first;
    auto allowed_x_end = road->allowed_x[position].second;

    if (current_x >= allowed_x_start && current_x <= allowed_x_end) {
      road->notify_add_x(this, position);
      return true;
    }
  }

  if (this->check_for_remove_x == true) {
    road->notify_remove_x(this, position);
  }
  return false;
}

bool Car::is_in_allowed_y(int position) {
  if (road->allowed_y.size()) {
    auto allowed_y_start = road->allowed_y[position].first;
    auto allowed_y_end = road->allowed_y[position].second;

    if (current_y >= allowed_y_start && current_y <= allowed_y_end) {
      road->notify_add_y(this, position);
      return true;
    }
  }

  if (this->check_for_remove_y == true) {
    road->notify_remove_y(this, position);
  }

  return false;
}

Car *Car::nearest_car(Axis axis) {
  auto found_car = road->find_nearest_car(this, axis);

  if (found_car) {
    if (!((axis == Axis::x_negative || axis == Axis::x_positive) &&
          abs(found_car->current_x - current_x) > 5) &&
        !((axis == Axis::y_negative || axis == Axis::y_positive) &&
          abs(found_car->current_y - current_y) > 3))
      return found_car;
  }

  return nullptr;
}

bool Car::is_near_start(Axis axis, float current_point, int overhead) {
  switch (axis) {
  case Axis::x_positive:
    if (current_point + overhead >= road->allowed_x[0].first &&
        current_point < road->allowed_x[0].first)
      return true;
    break;
  case Axis::x_negative:
    if (current_point - overhead <= road->allowed_x[1].second &&
        current_point > road->allowed_x[1].first)
      return true;
    break;

  case Axis::y_positive:
    if (current_point + overhead / 2 >= road->allowed_y[0].first &&
        current_point < road->allowed_y[0].first)
      return true;
    break;
  case Axis::y_negative:
    if (current_point - overhead / 2 <= road->allowed_y[1].second &&
        current_point > road->allowed_y[1].first)
      return true;
    break;

  default:
    return false;
    break;
  }

  return false;
}

void Car::check_for_sleep(Axis axis, int current_point) {
  unique_lock<mutex> lk(mtx);

  condition_variable *cv;
  queue<Car *> *q;

  switch (axis) {
  case Axis::x_positive:
    cv = &road->cvx0;
    q = &road->qx[0];
    break;

  case Axis::x_negative:
    cv = &road->cvx1;
    q = &road->qx[1];
    break;

  case Axis::y_positive:
    cv = &road->cvy0;
    q = &road->qy[0];
    break;

  case Axis::y_negative:
    cv = &road->cvy1;
    q = &road->qy[1];
    break;

  default:
    break;
  }

  Car *car;

  if (road->is_blocked(axis)) {
    if (q->size() == 0) {
      if (is_near_start(axis, current_point)) {
        road->add_to_queue(this, axis);
        cv->wait(lk, [&] { return !road->is_blocked(axis); });
        road->remove_from_queue(axis);
      }
    } else {
      car = nearest_car(axis);

      if (car != nullptr) {
        cv->wait(lk, [&] { return !road->is_blocked(axis); });
      }
    }
  }
}