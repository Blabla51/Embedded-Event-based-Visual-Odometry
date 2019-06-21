#include "hough_thread.h"

HoughThread::HoughThread(unsigned int hough_map_x,unsigned int hough_map_y, double zone_x, double zone_y, double threshold,unsigned int camera_x,unsigned int camera_y, unsigned int pc_exp_range) { // @suppress("Class members should be properly initialized")
	//sharedPrint("Initialisation of Hough Thread");
	this->m_thread = std::thread();

	this->m_camera_x = camera_x;
	this->m_camera_y = camera_y;
	this->m_hough_map_x = hough_map_x;
	this->m_hough_map_y = hough_map_y;
	this->m_last_input_event_timestamp = 0;

	this->m_pc_exp_range = pc_exp_range;
	this->m_threshold = threshold;
	this->m_zone_x = zone_x;
	this->m_zone_y = zone_y;
	this->m_hough_map = new double*[this->m_hough_map_x];
	for(unsigned int i = 0; i<this->m_hough_map_x; i++)
	{
		this->m_hough_map[i] = new double[this->m_hough_map_y];
		for(unsigned j = 0; j < this->m_hough_map_y; j++)
		{
			this->m_hough_map[i][j] = 0.0;
		}
	}
	this->m_hough_time_map = new double*[this->m_hough_map_x];
	for(unsigned int i = 0; i<this->m_hough_map_x; i++)
	{
		this->m_hough_time_map[i] = new double[this->m_hough_map_y];
		for(unsigned j = 0; j < this->m_hough_map_y; j++)
		{
			this->m_hough_time_map[i][j] = 0.0;
		}
	}
	this->m_look_up_dist = new double**[	this->m_camera_x];
	for(unsigned int i = 0; i<	this->m_camera_x; i++)
	{
		this->m_look_up_dist[i] = new double*[	this->m_camera_y];
		for(unsigned int j = 0; j<	this->m_camera_y; j++)
		{
			this->m_look_up_dist[i][j] = new double[2];
			this->m_look_up_dist[i][j][0] = 0.0; // SET CORRECT VALUE
			this->m_look_up_dist[i][j][1] = 0.0; // SET CORRECT VALUE
		}
	}
	this->m_pc_theta = new double[this->m_hough_map_x];
	this->m_pc_cos = new double[this->m_hough_map_x];
	this->m_pc_sin = new double[this->m_hough_map_x];
	for(unsigned int i = 0; i < this->m_hough_map_x; i++)
	{
		this->m_pc_theta[i] = (double)(i)*PI/((double)this->m_hough_map_x);
		this->m_pc_cos[i] = cos(this->m_pc_theta[i]);
		this->m_pc_sin[i] = sin(this->m_pc_theta[i]);
		if(i == 1)
		{
			std::cout << "Cos: " << this->m_pc_cos[i] << "; Sin: " << this->m_pc_sin[i] << "; Theta:" << this->m_pc_theta[i] << std::endl;
		}
	}
	this->m_rho_max = 209.3036; // Calculer le rho max
	this->m_decay = 200*1e-6;
	this->m_pc_exp = new double[1000000]; // Déterminer le nombre max de l'exp calculé
	for(unsigned int i = 0; i < this->m_pc_exp_range; i++)
	{
		m_pc_exp[i] = exp(-this->m_decay*(double)(i));
	}
}

void HoughThread::threadFunction() {
	this->mutexLog.lock();
	std::cout << "Doing my things ! Hough" << std::endl;
	this->mutexLog.unlock();
	std::unique_lock<std::mutex> lck(this->m_main_loop_mutex);
	bool tmp_stop = false;
	std::chrono::steady_clock::time_point begin;
	std::this_thread::sleep_for(std::chrono::microseconds(2500));
	std::chrono::steady_clock::time_point end;
	unsigned int event_counter = 0;
	do {
		while(this->m_ev_queue.empty())
		{
			this->mutexLog.lock();
			std::cout << "Waiting event " << std::endl;
			this->mutexLog.unlock();
			this->m_main_loop_cv.wait(lck);
		}
		this->m_ev_add_mutex.lock();
		Event e = this->m_ev_queue.front();
		this->m_ev_queue.pop();
		this->m_ev_add_mutex.unlock();
		switch(e.a)
		{
		case 1:
			if(event_counter++ == 0)
			{
				 begin = std::chrono::steady_clock::now();
			}
			this->computeEvent(e.x,e.y,e.t);
			if(event_counter == 1000000)
			{
				end = std::chrono::steady_clock::now();
			}
//			this->mutexLog.lock();
//			std::cout << "Adding event " << e.x << " " << e.y << std::endl;
//			this->mutexLog.unlock();
			break;
		case 2:
			tmp_stop = true;
			break;
		default:
			this->mutexLog.lock();
			std::cout << "Unknown Hough event: " << e.a << std::endl;
			this->mutexLog.unlock();
		}
	}while(!tmp_stop);
	this->mutexLog.lock();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<std::endl;
	this->mutexLog.unlock();
	//this->computeEvent(110,37,0);
	//this->printHoughMap();
	this->mutexLog.lock();
	std::cout << "Stopped my things ! Hough" << std::endl;
	this->mutexLog.unlock();
}


HoughThread::~HoughThread() {

}


int HoughThread::computeEvent(unsigned int x, unsigned int y, unsigned int timestamp)
{
	int nbr_event_generated = 0;
	for(unsigned int theta_index = 0; theta_index < this->m_hough_map_x; theta_index++)
	{
		if(this->m_tracking)
		{

		}
		double rho = (double)((int)x - (int)(this->m_camera_x >> 1))*this->m_pc_cos[theta_index]+(double)((int)y - (int)(this->m_camera_y >> 1))*this->m_pc_sin[theta_index];
		unsigned int rho_index = (unsigned int)((int)round(rho/this->m_rho_max*(double)(this->m_hough_map_y)) + (int)(this->m_hough_map_y >> 1));
//		this->mutexLog.lock();
//		if(i == 1)
//		{
//			std::cout << "Computed rho: " << rho << std::endl;
//			std::cout << "Computed rho index: " << rho_index << std::endl;
//			std::cout << "test: " << rho/this->m_rho_max*(double)(this->m_hough_map_y) << std::endl;
//		}
//		this->mutexLog.unlock();
		if(rho_index < this->m_hough_map_y)
		{
			this->m_hough_map[theta_index][rho_index] = this->m_hough_map[theta_index][rho_index]*this->getPCExp(timestamp-this->m_hough_time_map[theta_index][rho_index]) + 1.0;
			if(this->m_hough_map[theta_index][rho_index] >= this->m_threshold)
			{
				if(this->m_tracking)
				{
					for(int i = -this->m_zone_x; i <= this->m_zone_x; i++)
					{
						for(int j = -this->m_zone_y; j <= this->m_zone_y; j++)
						{
							if(rho_index+j < 0)
							{
								this->m_hough_map[(theta_index+i)%this->m_hough_map_x][-rho_index+j+1] = 0.0;
							}
							else
							{
								this->m_hough_map[(theta_index+i)%this->m_hough_map_x][rho_index+j] = 0.0;
							}
						}
					}
				}
				else
				{
					this->m_hough_map[theta_index][rho_index] = 0.0;
				}
//				this->mutexLog.lock();
//				std::cout << "Emit event" << std::endl;
//				this->mutexLog.unlock();
			}
		}
		this->m_hough_time_map[theta_index][rho_index] = timestamp;
	}
	this->m_last_input_event_timestamp = timestamp;
	return nbr_event_generated;
}

void HoughThread::printHoughMap()
{
	this->mutexLog.lock();
	for(unsigned int i = 0; i < this->m_hough_map_y; i++)
	{
		std::cout << i << ": " ;
		for(unsigned int j = 0; j < this->m_hough_map_x; j++)
		{
			std::cout << this->m_hough_map[j][i] << " ";
		}
		std::cout << std::endl;
	}
	this->mutexLog.unlock();
}

void HoughThread::addEvent(unsigned int x, unsigned int y, bool p, unsigned int t)
{
	this->m_ev_add_mutex.lock();
	this->m_ev_queue.push(Event(x,y,p,t,1));
	this->m_ev_add_mutex.unlock();
	this->m_main_loop_cv.notify_all();
}

void HoughThread::stop()
{
	this->m_ev_add_mutex.lock();
	this->m_ev_queue.push(Event(0,0,0,0,2));
	this->m_ev_add_mutex.unlock();
	BaseThread::stop();
}

double HoughThread::getPCExp(unsigned int dt)
{
	if(dt > this->m_pc_exp_range)
		return 0;
	else
		return this->m_pc_exp[dt];

}

void HoughThread::activateTracking()
{
	this->m_tracking = true;
}

void HoughThread::setPNPThread(PNPThread* pnpt)
{
	this->m_pnpt = pnpt;
}
