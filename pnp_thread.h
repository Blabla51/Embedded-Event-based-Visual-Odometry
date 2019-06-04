#ifndef PNP_THREAD_H
#define PNP_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

class PNP_Thread
{
private:
    std::thread m_thread;
 
public:
    PNP_Thread();
    void launch();
    void stop();
};

#endif
