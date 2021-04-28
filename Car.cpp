#include <ncurses.h>
#include <random>
#include <mutex>
#include "Car.h"
#include "Road.h"

Car::Car(int number, Road *road) {
    this->number = number;
    this->road = road;

    random_device rd;
    mt19937 rng(rd());
    uniform_real_distribution<> dist(1, 3);
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
        drive_forward(road_max_x, true, 1.4);
        drive_forward(road_max_y, false, 0.8);
        drive_backward(PADDING_X, true, 1.4);
        drive_backward(PADDING_Y, false, 0.8);

        loop++;
    }
    finished = true;
}

void Car::drive_forward(int max_point, bool axis, float multiplier) {
    float temp_speed;
    float *current_point;

    if (axis)
        current_point = &this->current_x;
    else
        current_point = &this->current_y;

    while (*current_point < max_point && !road->stop_flag) {
        if (is_in_allowed_x(0) || is_in_allowed_y(0))
            temp_speed = speed;
        else if (axis) {
            auto found_car = lookahead(true, true);
            if ((found_car && found_car->current_x - current_x > 6) || !found_car)
                temp_speed = speed;
            else
                temp_speed = base_speed;
        } else {
            auto found_car = lookahead(true, false);
            if ((found_car && found_car->current_y - current_y > 3) || !found_car)
                temp_speed = speed;
            else
                temp_speed = base_speed;
        }

        *current_point += temp_speed * multiplier;

        if (*current_point > max_point)
            *current_point = max_point;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Car::drive_backward(int min_point, bool axis, float multiplier) {
    float temp_speed;
    float *current_point;

    if (!axis)
        current_point = &this->current_y;
    else
        current_point = &this->current_x;

    while (*current_point > min_point && !road->stop_flag) {
        if (is_in_allowed_x(1) || is_in_allowed_y(1))
            temp_speed = speed;
        else if (axis) {
            auto found_car = lookahead(false, true);
            if ((found_car && (current_x - found_car->current_x > 6)) || !found_car)
                temp_speed = speed;
            else
                temp_speed = base_speed;
        } else {
            auto found_car = lookahead(false, false);
            if ((found_car && (abs(found_car->current_y - current_y) > 3)) || !found_car)
                temp_speed = speed;
            else
                temp_speed = base_speed;
        }

        *current_point -= temp_speed * multiplier;

        if (*current_point < min_point)
            *current_point = min_point;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/**
 * @param is_moving_forward is car is moving forward or backwards on axis
 * @param is_x_axis is axis x or y
 * @return found found car
 * **/
Car *Car::lookahead(bool is_moving_forward, bool is_x_axis) {
    return road->find_nearest_car(this, is_moving_forward, is_x_axis);
}

bool Car::is_in_allowed_x(int position) {
    auto allowed_x_start = road->allowed_x[position].first;
    auto allowed_x_end = road->allowed_x[position].second;

    if (current_x >= allowed_x_start && current_x <= allowed_x_end)
        return true;

    return false;
}

bool Car::is_in_allowed_y(int position) {
    auto allowed_y_start = road->allowed_y[position].first;
    auto allowed_y_end = road->allowed_y[position].second;

    if (current_y >= allowed_y_start && current_y <= allowed_y_end)
        return true;

    return false;
}
