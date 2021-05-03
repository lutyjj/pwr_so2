#pragma once

#include <thread>

#define PADDING_X 2
#define PADDING_Y 1
#define LOOPS 3

using namespace std;

class Road;

enum class Axis {
    x_positive, x_negative, y_positive, y_negative
};

class Car
{
private:
    int loop = 0;
    thread *t;
    Road *road;
    Axis axis;

    void thread_func();
    void drive_forward(int max_x, Axis axis, float mult_x);
    // void drive_backward(int max_x, bool axis, float mult_x);

    bool is_in_allowed_x(int position);
    bool is_in_allowed_y(int position);
    float nearest_car_speed(Axis axis);

public:
    Car(int number, Road *road);
    ~Car();

    float speed = 0;
    float base_speed = 2;
    float current_x = 0 + PADDING_X;
    float current_y = 0 + PADDING_Y;
    int number = 0;
    bool finished = false;
};
