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
//void sharedPrint(std::string msg);

#endif /* COMMON_H_ */
