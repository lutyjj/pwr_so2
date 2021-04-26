#pragma once
#include <thread>
#include <mutex>

#define PADDING_X 2
#define PADDING_Y 1
#define LOOPS 3

using namespace std;

struct Road;

struct Car
{
    int current_x = 0 + PADDING_X;
    int current_y = 0 + PADDING_Y;
    float speed = 0;
    int number;
    int loop = 0;
    bool finished = false;
    thread *t;
    Road* road;

    Car(int number, Road* road);
    void thread_func();
};