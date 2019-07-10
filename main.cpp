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
#if OS == OS_LINUX
#include <curl/curl.h>
#endif
#include "uart_thread.h"
#include "hough_thread.h"
#include "pnp_thread.h"
//#include "user_thread.h"
#include "common.h"
//using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

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

	bool stop = false;
#if OS == OS_LINUX
	CURL *curl;
	CURLcode res;

	//static const char *postthis = "moo mooo moo moo";
	std::string readBuffer;

	/*if(!curl)
	{
		//BaseThread::mutexLog.lock();
		std::cout << "CURL init failed" << std::endl;
		//BaseThread::mutexLog.unlock();
	}
	else
	{
	}*/
#endif
	while(stop == false)
	{
	#if OS == OS_LINUX
	//	const int fd = fileno(stdin);
	//	const int fcflags = fcntl(fd,F_GETFL);
	//	if (fcflags<0) { /* handle error */}
	//	if (fcntl(fd,F_SETFL,fcflags | O_NONBLOCK) <0) { /* handle error */}
	//	std::cout << "Char:" << stdin->getchar() << std::endl;


		curl = curl_easy_init();
		std::string data = "Data";
		data.append(pnp_thread_object->generateWebServerData());
		readBuffer.clear();
		static const char* post_data1 = data.c_str();
		curl_easy_setopt(curl, CURLOPT_URL, "http://10.0.1.56/PFE/raspi_client.php");
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data1);
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_data1.length());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	    curl_easy_setopt(curl, CURLOPT_POST, 1);
		res = curl_easy_perform(curl);
		std::cout << "Send: " << post_data1 << " " << res << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//BaseThread::mutexLog.lock();
		std::cout << readBuffer << std::endl;

		if(readBuffer == std::string("stop"))
		{
			stop = true;
		}
		//BaseThread::mutexLog.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		break;
	#elif OS == OS_WINDOWS
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		break;
	#endif
	}
#if OS == OS_LINUX
	curl_easy_cleanup(curl);
#endif
	std::cout << "Stopped objects " << std::endl;
	uart_thread_object->stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "Stopped objects " << std::endl;
	hough_thread_object->stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "Stopped objects " << std::endl;
	pnp_thread_object->stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "Stopped objects " << std::endl;
	delete uart_thread_object;
	delete hough_thread_object;
	delete pnp_thread_object;

    return EXIT_SUCCESS;
}
