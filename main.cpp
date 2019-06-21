#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>
//#include "mingw.thread.h"
#include <thread>
//#include <mutex>
#include <chrono>
#include "uart_thread.h"
#include "hough_thread.h"
#include "pnp_thread.h"
//#include "user_thread.h"
#include "common.h"
//using namespace std;


int main(int argc, char *argv[])
{
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";

	PNPThread *pnp_thread_object = new PNPThread();
	HoughThread *hough_thread_object = new HoughThread(256,512);
    UARTThread *uart_thread_object = new UARTThread();

    uart_thread_object->setHoughThread(hough_thread_object);

	pnp_thread_object->start();
	hough_thread_object->start();
	uart_thread_object->start();

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	uart_thread_object->stop();
	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	hough_thread_object->stop();
	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	pnp_thread_object->stop();
	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	delete uart_thread_object;
	delete hough_thread_object;
	delete pnp_thread_object;

    return EXIT_SUCCESS;
}
