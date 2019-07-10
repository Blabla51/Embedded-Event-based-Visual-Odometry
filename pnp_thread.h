#ifndef PNP_THREAD_H
#define PNP_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <algorithm>
#include "common.h"
#include "base_thread.h"
#include <future>
//#include <sstream>
//#include "libusb.h"

class HoughThread;

class PNPThread : public BaseThread
{
private:
	double 		m_focal_length;
	double** 	m_epsilon;
	double** 	m_object_points;
	double** 	m_object_matrix;
	int			m_nbr_lines_identified;
	double**	m_line_parameters;
	double**	m_line_inters;
	int**	m_filtering_array;
	double 		m_confidence_coef;
	int** 		m_current_filter_centers;
	int			m_ht_rho_max;
	int 		m_ht_map_x;
	int 		m_ht_map_y;
	//std::stringstream m_web_string_stream;

	std::queue<HoughEvent>	m_ev_queue;
	std::mutex 			m_ev_add_mutex;
	std::mutex 			m_filter_mutex;
	std::mutex 			m_web_mutex;
	HoughThread*		m_ht;

	void multMat(double** m1, double** m2, double** res, int ligne, int inter, int colonne);
	void transMat(double** matrix, double** res, int ligne, int colonne);
	void dispMat(double** m1,int ligne, int colonne);

public:
	void threadFunction();
	PNPThread(double focal_length,HoughThread* ht);
    ~PNPThread();

    void stop();

    void addEvent(double theta, double dist, unsigned int t, int line_id);
    void computeEvent(double theta, double dist, unsigned int t, int line_id);
    void computeLineIntersection();
    void computePosit();
    void updateFilteringArray();
    void updateLineParameters(double theta, double dist, bool rotated, int line_id);
    int getFilterValue(int t, int d);
    void printFilteringMap();
    std::string generateWebServerData();
};

#endif
