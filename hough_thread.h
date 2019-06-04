#ifndef HOUGH_THREAD_H
#define HOUGH_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
//#include "mingw.thread.h"
#include <thread>
#include <mutex>
#include "pnp_thread.h"


class Hough_Thread
{
private:
    int m_value = 0;
 
public:
    Calc(int value=0): m_value(value) {}
 
    Calc& add(int value) { m_value  += value;  return *this; }
    Calc& sub(int value) { m_value -= value;  return *this; }
    Calc& mult(int value) { m_value *= value;  return *this; }
 
    int getValue() { return m_value ; }
};

#endif