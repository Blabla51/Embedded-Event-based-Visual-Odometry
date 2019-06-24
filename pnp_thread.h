#ifndef PNP_THREAD_H
#define PNP_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include "common.h"
#include "base_thread.h"
//#include "libusb.h"

class HoughThread;

class PNPThread : public BaseThread
{
private:
	double 		m_focal_length;
	double** 	m_epsilon;
	double** 	m_object_points;
	double** 	m_object_matrix;

	std::queue<HoughEvent>	m_ev_queue;
	std::mutex 			m_ev_add_mutex;
	HoughThread*		m_ht;

	void multMat(double** m1, double** m2, double** res, int ligne, int inter, int colonne);
	void transMat(double** matrix, double** res, int ligne, int colonne);
	void dispMat(double** m1,int ligne, int colonne);

public:
	void threadFunction();
	PNPThread(double focal_length);
    ~PNPThread();

    void stop();

    void addEvent(double theta, double dist, unsigned int t, int line_id);
    void computeEvent(double theta, double dist, unsigned int t, int line_id);
    void computePosit(double** image_points);
    void setHoughThread(HoughThread* ht);
};

#endif
