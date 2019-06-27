/*
 * common.h
 *
 *  Created on: 4 juin 2019
 *      Author: Johan
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#define PI 3.14159265

#define DEBUG_NO 0
#define DEBUG_YES 1

#define WINDOWS 0
#define LINUX 1

#define MODE_ONLINE 0
#define MODE_OFFLINE 1

#define DEBUG DEBUG_YES
#define OS WINDOWS
#define MODE MODE_OFFLINE

class Event {
public:
	unsigned int x;
	unsigned int y;
	bool p;
	unsigned int t;
	unsigned int a;
	Event(unsigned int x, unsigned int y,bool p, unsigned int t,unsigned int a)
	{
		this->x = x;
		this->y = y;
		this->p = p;
		this->t = t;
		this->a = a;
	}
};

class HoughEvent {
public:
	double theta;
	double dist;
	unsigned int t;
	int line_id;
	unsigned int a;
	HoughEvent(double theta, double dist,unsigned int t, int line_id,unsigned int a)
	{
		this->theta = theta;
		this->dist = dist;
		this->t = t;
		this->line_id = line_id;
		this->a = a;
	}
};
//void sharedPrint(std::string msg);

#endif /* COMMON_H_ */
