#include "Road.h"
#include "Car.h"

#include <ncurses.h>
#include <algorithm>
#include <random>


Road::Road(int x, int y) {
  this->x = x;
  this->y = y;
  this->stop_flag = false;

  this->allowed_x.emplace_back(x / 4, x - x / 4);
  this->allowed_x.emplace_back(x / 5, x / 2);
  this->allowed_y.emplace_back(y / 4, y / 2);
  this->allowed_y.emplace_back(y / 3, y - y / 3);

  this->allowed_car_amount_x.emplace_back(2);
  this->allowed_car_amount_x.emplace_back(2);
  this->allowed_car_amount_y.emplace_back(2);
  this->allowed_car_amount_y.emplace_back(2);

  this->cars_in_allowed_x.emplace_back();
  this->cars_in_allowed_x.emplace_back();
  this->cars_in_allowed_y.emplace_back();
  this->cars_in_allowed_y.emplace_back();

  this->qx.emplace_back();
  this->qx.emplace_back();
  this->qy.emplace_back();
  this->qy.emplace_back();

  this->blocked_segments_x.emplace_back(false);
  this->blocked_segments_x.emplace_back(false);
  this->blocked_segments_y.emplace_back(false);
  this->blocked_segments_y.emplace_back(false);

  this->t_spawn_car = new thread([this]() { spawn_car(); });
  this->t_allowed_road_watcher = new thread([this]() { watch_segments(); });
}

Road::~Road() {
  clear();
  addstr("Waiting for threads...\n");
  refresh();

  t_spawn_car->join();

  for (auto car : cars)
    delete car;

  cars.clear();
}

void Road::draw() {
  draw_rectangle(0, 0, y - 1, x - 1);
  draw_rectangle(2, 4, y - 3, x - 5);

  if (allowed_x.size() && allowed_y.size()) {
    draw_green_rectangle(allowed_x[0].first + 1, allowed_x[0].second + 2, 1, 2);
    draw_green_rectangle(x - 4, x - 1, allowed_y[0].first, allowed_y[0].second);
    draw_green_rectangle(allowed_x[1].first + 1, allowed_x[1].second + 2, y - 2,
                         y - 1);
    draw_green_rectangle(1, 4, allowed_y[1].first, allowed_y[1].second);
  }

  mtx.lock();
  for (int i = 0; i < cars.size(); i++) {
    auto car = cars[i];

    if (car->finished)
      cars.erase(cars.begin() + i);
    else
      mvprintw(car->current_y, car->current_x, "%d", car->number);
  }
  mtx.unlock();
}

void Road::draw_rectangle(int y1, int x1, int y2, int x2) {
  mvhline(y1, x1, 0, x2 - x1);
  mvhline(y2, x1, 0, x2 - x1);
  mvvline(y1, x1, 0, y2 - y1);
  mvvline(y1, x2, 0, y2 - y1);
  mvaddch(y1, x1, ACS_ULCORNER);
  mvaddch(y2, x1, ACS_LLCORNER);
  mvaddch(y1, x2, ACS_URCORNER);
  mvaddch(y2, x2, ACS_LRCORNER);
}

void Road::draw_green_rectangle(int x1, int x2, int y1, int y2) {
  for (int i = x1; i < x2; i++) {
    for (int j = y1; j < y2; j++) {
      mvaddch(j, i, ' ' | COLOR_PAIR(1));
    }
  }
}

void Road::spawn_car() {
  random_device rd;
  mt19937 rng(rd());
  uniform_int_distribution<> dist(2000, 4000);

  int count = 0;
  while (!this->stop_flag && cars.size() < 10) //  && cars.size() < 10
  {
    count++;
    cars.push_back(new Car(count, this));
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
  }
}

void Road::watch_segments() {
  while (!this->stop_flag) {
    for (int i = 0; i < cars_in_allowed_x.size(); i++) {
      if (cars_in_allowed_x[i].size() >= allowed_car_amount_x[i]) {
        blocked_segments_x[i] = true;
      }
      else {
        blocked_segments_x[i] = false;
      }
    }

    for (int i = 0; i < cars_in_allowed_y.size(); i++) {
      if (cars_in_allowed_y[i].size() >= allowed_car_amount_y[i])
        blocked_segments_y[i] = true;
      else {
        blocked_segments_y[i] = false;
      }
    }
  }
}

Car *Road::find_nearest_car(Car *param_car, Axis axis) {
  Car *nearest_car = nullptr;
  float prev_nearest_max = param_car->base_speed * 100;
  float prev_nearest_min = -1;

  mtx.lock();

  if (axis == Axis::x_negative || axis == Axis::x_positive) {
    for (auto &car : cars) {
      if (car->current_x == param_car->current_x)
        continue;

      if (axis == Axis::x_positive) {
        if (car->current_x > param_car->current_x &&
            car->current_x < prev_nearest_max &&
            car->current_y == param_car->current_y) {
          prev_nearest_max = car->current_x;
          nearest_car = car;
        }
      } else {
        if (car->current_x < param_car->current_x &&
            car->current_x > prev_nearest_min &&
            car->current_y == param_car->current_y) {
          prev_nearest_min = car->current_x;
          nearest_car = car;
        }
      }
    }
  } else {
    for (auto &car : cars) {
      if (car->current_y == param_car->current_y)
        continue;

      if (axis == Axis::y_positive) {
        if (car->current_y > param_car->current_y &&
            car->current_y < prev_nearest_max &&
            car->current_x == param_car->current_x) {
          prev_nearest_max = car->current_y;
          nearest_car = car;
        }
      } else {
        if (car->current_y < param_car->current_y &&
            car->current_y > prev_nearest_min &&
            car->current_x == param_car->current_x) {
          prev_nearest_min = car->current_y;
          nearest_car = car;
        }
      }
    }
  }

  mtx.unlock();

  return nearest_car;
}

void Road::stop() {
  stop_flag = true;
  mtx.unlock();

  delete this;
}

void Road::notify_add_x(Car *car, int position) {
  if (find(cars_in_allowed_x[position].begin(),
           cars_in_allowed_x[position].end(),
           car) == cars_in_allowed_x[position].end()) {
    cars_in_allowed_x[position].push_back(car);

    car->check_for_remove_x = true;
  }
}

void Road::notify_remove_x(Car *car, int position) {
  cars_in_allowed_x[position].erase(
      std::remove(cars_in_allowed_x[position].begin(),
                  cars_in_allowed_x[position].end(), car),
      cars_in_allowed_x[position].end());

  car->check_for_remove_x = false;

  switch (position)
  {
  case 0:
    cvx0.notify_one();
    break;
  
  case 1:
    cvx1.notify_one();
    break;

  default:
    break;
  }
}

void Road::notify_add_y(Car *car, int position) {
  if (find(cars_in_allowed_y[position].begin(),
           cars_in_allowed_y[position].end(),
           car) == cars_in_allowed_y[position].end()) {
    cars_in_allowed_y[position].push_back(car);

    car->check_for_remove_y = true;
  }
}

void Road::notify_remove_y(Car *car, int position) {
  cars_in_allowed_y[position].erase(
      std::remove(cars_in_allowed_y[position].begin(),
                  cars_in_allowed_y[position].end(), car),
      cars_in_allowed_y[position].end());

  car->check_for_remove_y = false;

  switch (position)
  {
  case 0:
    cvy0.notify_one();
    break;
  
  case 1:
    cvy1.notify_one();
    break;

  default:
    break;
  }
}

bool Road::is_blocked(Axis axis) {
  switch (axis) {
  case Axis::x_positive:
    return blocked_segments_x[0];
    break;

  case Axis::x_negative:
    return blocked_segments_x[1];
    break;

  case Axis::y_positive:
    return blocked_segments_y[0];
    break;

  case Axis::y_negative:
    return blocked_segments_y[1];
    break;

  default:
    break;
  }

  return false;
}


void Road::add_to_queue(Car* car, Axis axis) {
  switch (axis)
  {
  case Axis::x_positive:
    return qx[0].push(car);
    break;

  case Axis::x_negative:
    return qx[1].push(car);
    break;

  case Axis::y_positive:
    return qy[0].push(car);
    break;

  case Axis::y_negative:
    return qy[1].push(car);
    break;
  
  default:
    break;
  }
}

void Road::remove_from_queue(Axis axis) {
    switch (axis)
  {
  case Axis::x_positive:
    return qx[0].pop();
    break;

  case Axis::x_negative:
    return qx[1].pop();
    break;

  case Axis::y_positive:
    return qy[0].pop();
    break;

  case Axis::y_negative:
    return qy[1].pop();
    break;
  
  default:
    break;
  }
}