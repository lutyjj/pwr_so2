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
    float speed = 0;
    int loop = 0;
    thread *t;
    Road *road;

public:
    Car(int number, Road *road);
    ~Car();

    int current_x = 0 + PADDING_X;
    int current_y = 0 + PADDING_Y;
    int number = 0;
    bool finished = false;
};