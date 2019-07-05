#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>
//#include "mingw.thread.h"
#include <thread>
//#include <mutex>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
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

    UARTThread *uart_thread_object = new UARTThread();
	HoughThread *hough_thread_object = new HoughThread(512,256);
	PNPThread *pnp_thread_object = new PNPThread(151.0, hough_thread_object);

    hough_thread_object->setPNPThread(pnp_thread_object);
    uart_thread_object->setHoughThread(hough_thread_object);

	pnp_thread_object->start();
	hough_thread_object->start();
	uart_thread_object->start();
#if OS == OS_LINUX
	const int fd = fileno(stdin);
	const int fcflags = fcntl(fd,F_GETFL);
	if (fcflags<0) { /* handle error */}
	if (fcntl(fd,F_SETFL,fcflags | O_NONBLOCK) <0) { /* handle error */}
	cout << "Char:" << stdin.getchar() << std::endl;
#elif OS == OS_WINDOWS
	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
#endif
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
