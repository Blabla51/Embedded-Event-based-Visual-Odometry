#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
//#include "mingw.thread.h"
#include <thread>
//#include <mutex>
//#include "uart_thread.h"
//#include "hough_thread.h"
//#include "pnp_thread.h"
//#include "user_thread.h"
#include "common.h"
//using namespace std;

void tf_main() {
    sharedPrint("Thread MainT ID");
}

void tf_uart() {
    sharedPrint("Thread UART ID");
}

void tf_hough() {
    sharedPrint("Thread Hough ID");
}

void tf_pnp() {
    sharedPrint("Thread PNP ID");
}

int main(int argc, char *argv[])
{
    std::thread t_main(tf_main);
    std::thread t_uart(tf_uart);
    std::thread t_hough(tf_hough);
    std::thread t_pnp(tf_pnp);

    sharedPrint("Thread Main ID");

    t_pnp.join();
    t_hough.join();
    t_uart.join();
    t_main.join();

    return EXIT_SUCCESS;
}
