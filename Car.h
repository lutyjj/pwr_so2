#pragma once

#include <thread>

#define PADDING_X 2
#define PADDING_Y 1
#define LOOPS 3

using namespace std;

class Road;

class Car
{
private:
    int loop = 0;
    thread *t;
    Road *road;


    void thread_func();
    void drive_forward(int max_x, bool axis, float mult_x);
    void drive_backward(int max_x, bool axis, float mult_x);
    bool is_in_allowed_x(int position);
    bool is_in_allowed_y(int position);
    Car *lookahead(bool is_moving_forward, bool is_x_axis);

public:
    Car(int number, Road *road);
    ~Car();

    float speed = 0;
    float base_speed = 1;
    float current_x = 0 + PADDING_X;
    float current_y = 0 + PADDING_Y;
    int number = 0;
    bool finished = false;
};
