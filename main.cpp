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
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#endif
#include "uart_thread.h"
#include "hough_thread.h"
#include "pnp_thread.h"
//#include "user_thread.h"
#include "common.h"
//using namespace std;

#if SIMULINK_RETURN == 1
struct RPIt_socket_mes_struct	mes;
struct RPIt_socket_con_struct	con;
unsigned char					exit_req = 0;
#endif

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
	HoughThread *hough_thread_object = new HoughThread(256,128);
	PNPThread *pnp_thread_object = new PNPThread(151.0*1.285, hough_thread_object);

    hough_thread_object->setPNPThread(pnp_thread_object);
    uart_thread_object->setHoughThread(hough_thread_object);

	pnp_thread_object->start();
	hough_thread_object->start();
	uart_thread_object->start();

	bool stop = false;
#if OS == OS_LINUX
#if SIMULINK_RETURN == 1
	struct addrinfo 				hints;
	struct addrinfo 				*result, *rp;
	int 							sfd, s, i;
	struct sockaddr_storage 		peer_addr;
	socklen_t 						peer_addr_len;
	ssize_t 						nread;
	struct RPIt_socket_mes_struct	local_mes;
	struct RPIt_socket_con_struct	local_con;
	struct timespec 				current_time;

	/* Clear mes structure */

	mes.timestamp = 0;
	for ( i = 0; i < RPIT_SOCKET_MES_N; i++ )
		mes.mes[i] = 0.1 + i;
	mes.magic = RPIT_SOCKET_MAGIC;


	/* Clear con structure */

	con.magic = 0;
	con.timestamp = 0;
	for ( i = 0; i < RPIT_SOCKET_CON_N; i++ )
		con.con[i] = 0.0;

	memset( &hints, 0, sizeof( struct addrinfo ) );
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;					/* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo( NULL, RPIT_SOCKET_PORT, &hints, &result );

	if ( s != 0 ) {
		flockfile( stderr );
		fprintf( stderr, "rpit_socket_server: function getaddrinfo returned: %s\n", gai_strerror( s ) );
		funlockfile( stderr );
		exit( EXIT_FAILURE );
	 }

	/*
		getaddrinfo() returns a list of address structures.
		Try each address until we successfully bind(2).
		If socket(2) (or bind(2)) fails, we (close the socket
		and) try the next address.
	*/

	for ( rp = result; rp != NULL; rp = rp->ai_next ) {

		std::cout << "IP: " << rp->ai_addr << std::endl;

		sfd = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol );
		if ( sfd == -1 )
			continue;

		if ( bind( sfd, rp->ai_addr, rp->ai_addrlen ) == 0 )
			break;									/* Success */

		close( sfd );
	}

	if ( rp == NULL ) {					/* No address succeeded */
		flockfile( stderr );
		fprintf( stderr, "rpit_socket_server: could not bind. Aborting.\n" );
		funlockfile( stderr );
		exit( EXIT_FAILURE );
	}

	freeaddrinfo( result );
#endif
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
	#if SIMULINK_RETURN == 0
		//	const int fd = fileno(stdin);
		//	const int fcflags = fcntl(fd,F_GETFL);
		//	if (fcflags<0) { /* handle error */}
		//	if (fcntl(fd,F_SETFL,fcflags | O_NONBLOCK) <0) { /* handle error */}
		//	std::cout << "Char:" << stdin->getchar() << std::endl;


			curl = curl_easy_init();
			std::string data = pnp_thread_object->generateWebServerData();
			readBuffer.clear();
			//static const char* post_data1 = data.c_str();
			curl_easy_setopt(curl, CURLOPT_URL, "http://10.0.1.56/PFE/raspi_client.php");
			//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
			//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			res = curl_easy_perform(curl);
			//std::cout << "Send: " << data << " " << res << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//BaseThread::mutexLog.lock();
			//std::cout << readBuffer << std::endl;

			if(readBuffer == std::string("stop"))
			{
				stop = true;
			}
	#elif SIMULINK_RETURN == 1
			peer_addr_len = sizeof( struct sockaddr_storage );
			double* tmp[2];
			nread = recvfrom(	sfd, (char*)&tmp, sizeof(tmp), 0,
												(struct sockaddr *)&peer_addr, &peer_addr_len );

			std::cout << tmp << std::endl;
			//std::cout << nread << std::endl;

			// Memcopy is faster than socket read: avoid holding the mutex too long

			/*memcpy( &con, &local_con, sizeof( struct RPIt_socket_con_struct ) );

			if ( nread == -1 )	{
				flockfile( stderr );
				fprintf( stderr, "rpit_socket_server: function recvfrom exited with error.\n" );
				funlockfile( stderr );

				// Clear control in case of error

				for ( i = 0; i < RPIT_SOCKET_CON_N; i++ )
					con.con[i] = 0.0;
			}

			if ( nread != sizeof( struct RPIt_socket_con_struct ) )	{
				flockfile( stderr );
				fprintf( stderr, "rpit_socket_server: function recvfrom did not receive the expected packet size.\n" );
				funlockfile( stderr );

				// Clear control in case of error

				for ( i = 0; i < RPIT_SOCKET_CON_N; i++ )
					con.con[i] = 0.0;
			}

			if ( con.magic != RPIT_SOCKET_MAGIC )	{
				flockfile( stderr );
				fprintf( stderr, "rpit_socket_server: magic number problem. Expected %d but received %d.\n", RPIT_SOCKET_MAGIC, con.magic );
				funlockfile( stderr );

				// Clear control in case of error

				for ( i = 0; i < RPIT_SOCKET_CON_N; i++ )
					con.con[i] = 0.0;
			}

			for ( i = 0; i < RPIT_SOCKET_CON_N; i++ )
			{
				if(con.con[i] != 0.0)
				{
					std::cout << "CON" << i << " = " << con.con[i] << std::endl;
				}
			}*/

			// Critical section : copy of the measurements to a local variable
			/*clock_gettime( CLOCK_MONOTONIC, &current_time );

			// Critical section

			mes.timestamp = (unsigned long long)current_time.tv_sec * 1000000000
												+ (unsigned long long)current_time.tv_nsec;

			memcpy( &local_mes, &mes, sizeof( struct RPIt_socket_mes_struct ) );

			std::cout << local_mes.mes[5] << std::endl;

			// Send measurements to the socket

			if ( sendto(	sfd, (char*)&local_mes, sizeof( struct RPIt_socket_mes_struct ), 0,
										(struct sockaddr *)&peer_addr,
										peer_addr_len) != sizeof( struct RPIt_socket_mes_struct ) )	{
				flockfile( stderr );
				fprintf( stderr, "rpit_socket_server: error sending measurements.\n" );
				funlockfile( stderr );
			}*/
	#endif
		//BaseThread::mutexLog.unlock();
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		//break;
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
	hough_thread_object->printHoughMap();
	delete uart_thread_object;
	delete hough_thread_object;
	delete pnp_thread_object;

    return EXIT_SUCCESS;
}
