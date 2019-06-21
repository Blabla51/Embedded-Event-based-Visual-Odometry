#ifndef PNP_THREAD_H
#define PNP_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "common.h"
#include "base_thread.h"
//#include "libusb.h"

class PNPThread : public BaseThread
{
protected:

public:
	void threadFunction();
	PNPThread();
    ~PNPThread();

    void addEvent();
    void computePosit();
};

#endif
