#ifndef HOUGH_THREAD_H
#define HOUGH_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cmath>
#include <queue>
#include <atomic>
#include <fstream>
#include "common.h"
#include "base_thread.h"

class PNPThread;

class HoughThread : public BaseThread
{
private:
	int 	m_hough_map_x;
	int 	m_hough_map_y;
	int 	m_camera_x;
	int 	m_camera_y;
	int 	m_last_input_event_timestamp;
	int 	m_pc_exp_range;
	double 			m_threshold;
	double			m_decay;
	double 			m_rho_max;
	double 			m_zone_x;
	double 			m_zone_y;
	double** 		m_hough_map;
	double** 		m_hough_time_map;
	double*** 		m_look_up_dist;
	double* 		m_pc_theta;
	double* 		m_pc_cos;
	double* 		m_pc_sin;
	double* 		m_pc_exp;
	int*** 	m_pc_hough_coord;
	std::atomic<bool>	m_tracking;
	std::queue<Event>	m_ev_queue;
	std::mutex 			m_ev_add_mutex;
	PNPThread*		m_pnpt;


	double getPCExp(int dt);

public:
	HoughThread(int hough_map_x, int hough_map_y, double zone_x = 10, double zone_y = 10, double threshold = 15.0, int camera_x = 128, int camera_y = 128, int pc_exp_range = 1000000);
    ~HoughThread();

    void stop();
	void threadFunction();

	int computeEvent(unsigned int x, unsigned int y, unsigned int timestamp);
    void printHoughMap();
    void addEvent(unsigned int x, unsigned int y, bool p, unsigned int t);
    void activateTracking();
    void setPNPThread(PNPThread* pnpt);
};

#endif
