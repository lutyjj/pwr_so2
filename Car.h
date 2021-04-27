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
    void thread_func();
    int loop = 0;
    thread *t;
    Road *road;
    void drive_forward(int max_x, bool axis, float mult_x);
    void drive_backward(int max_x, bool axis, float mult_x);
    // 0 - top, 1 - bottom
    bool is_in_blocked_x(int position);
    // 0 - top, 1 - bottom
    bool is_in_blocked_y(int position);
    float lookahead();

public:
    Car(int number, Road *road);
    ~Car();

    float speed = 0;
    float current_x = 0 + PADDING_X;
    float current_y = 0 + PADDING_Y;
    int number = 0;
    bool finished = false;
};