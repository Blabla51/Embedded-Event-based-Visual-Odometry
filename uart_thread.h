/*
 * uart_thread.h
 *
 *  Created on: 12 juin 2019
 *      Author: Johan
 */

#ifndef UART_THREAD_H_
#define UART_THREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "common.h"
#include "base_thread.h"
#include "hough_thread.h"
//#include "libusb.h"

class UARTThread : public BaseThread
{
private:
	HoughThread* m_ht;

public:
    UARTThread();
    ~UARTThread();
	void threadFunction();
    void setHoughThread(HoughThread* ht);
};

#endif /* UART_THREAD_H_ */
