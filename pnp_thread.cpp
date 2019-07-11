#include "pnp_thread.h"
#include "hough_thread.h"

PNPThread::PNPThread(double fl, HoughThread* ht): m_web_string_stream(std::ios_base::in | std::ios_base::out | std::ios_base::ate)
{
	this->m_ht = ht;
	this->m_focal_length = fl;
	this->m_nbr_lines_identified = 0;
	this->m_confidence_coef = 0.15;
	this->m_current_filter_centers = new int*[4];
	this->m_web_string_stream << "[";

	this->m_ht_rho_max = ht->getRhoMax();
	this->m_ht_map_x = ht->getMapX();
	this->m_ht_map_y = ht->getMapY();

	for(int i = 0; i < 4; i++)
	{
		this->m_current_filter_centers[i] = new int[2];
		this->m_current_filter_centers[i][0] = 0;
		this->m_current_filter_centers[i][1] = 0;
	}
	this->m_line_parameters = new double*[4];
	for(int i = 0; i < 4; i++)
	{
		this->m_line_parameters[i] = new double[2];
		this->m_line_parameters[i][0] = 0.0;
		this->m_line_parameters[i][1] = 0.0;
	}

	this->m_filtering_array = new int*[this->m_ht_map_x];
	for(int i = 0; i < this->m_ht_map_x; i++)
	{
		this->m_filtering_array[i] = new int[this->m_ht_map_y];
		for(int j = 0; j < this->m_ht_map_y; j++)
		{
			this->m_filtering_array[i][j] = 0;
		}
	}

	this->m_line_inters = new double*[4];
	for(int i = 0; i < 4; i++)
	{
		this->m_line_inters[i] = new double[2];
		this->m_line_inters[i][0] = 0.0;
		this->m_line_inters[i][1] = 0.0;
	}

	this->m_epsilon = new double*[1];
	this->m_epsilon[0] = new double[4];
	for(int i = 0; i < 4; i++)
	{
		m_epsilon[0][i] = 0.0;
	}
	this->m_object_points = new double*[3];
	for(int i = 0; i < 3; i++)
	{
		this->m_object_points[i] = new double[4];
		for(int j = 0; j < 4; j++)
		{
			m_object_points[i][j] = 0.0;
		}
	}
	m_object_points[0][1] = -0.057447;
	m_object_points[1][1] = -0.0995;
	m_object_points[2][1] = -0.041032;
	m_object_points[0][2] =  0.114893;
	m_object_points[1][2] = -0.0;
	m_object_points[2][2] = -0.041032;
	m_object_points[0][3] = -0.057447;
	m_object_points[1][3] =  0.0995;
	m_object_points[2][3] = -0.041032;
	this->m_object_matrix = new double*[4];
	for(int i = 0; i < 4; i++)
	{
		this->m_object_matrix[i] = new double[3];
		for(int j = 0; j < 3; j++)
		{
			m_object_matrix[i][j] = 0.0;
		}
	}
	m_object_matrix[0][0] = -2.42789526240074e-21;
	m_object_matrix[1][0] = -2.90124173146107;
	m_object_matrix[2][0] =  5.80248346292213;
	m_object_matrix[3][0] = -2.90124173146107;
	m_object_matrix[0][1] = -3.94495592634115e-16;
	m_object_matrix[1][1] = -5.02512562814070;
	m_object_matrix[2][1] =  5.88950464670508e-16;
	m_object_matrix[3][1] =  5.02512562814070;
	m_object_matrix[0][2] =  1.17162378533812e-15;
	m_object_matrix[1][2] = -8.12371725123699;
	m_object_matrix[2][2] = -8.12378795804464;
	m_object_matrix[3][2] = -8.12371725123699;

	// FORCE BEGIN

	this->m_nbr_lines_identified = 4;
	//4.22, 16
	this->m_current_filter_centers[0][0] = 344;
	this->m_current_filter_centers[0][1] = 20;
	//5.20326, 9.82994
	this->m_current_filter_centers[1][0] = 424;
	this->m_current_filter_centers[1][1] = 12;
	//2.66299, 35.539
	this->m_current_filter_centers[2][0] = 217;
	this->m_current_filter_centers[2][1] = 45;
	//0.55, 15
	this->m_current_filter_centers[3][0] = 45;
	this->m_current_filter_centers[3][1] = 19;

	this->computeLineIntersection();
}

PNPThread::~PNPThread() {

}

void PNPThread::threadFunction() {
	this->m_is_launched = true;
	this->mutexLog.lock();
	std::cout << "Doing my things ! PNP" << std::endl;
	this->mutexLog.unlock();
	std::unique_lock<std::mutex> lck(this->m_main_loop_mutex);
	/*double** image_points = new double*[2];
	for(int i = 0; i < 2; i++)
	{
		image_points[i] = new double[4];
	}
	image_points[0][0] = 9.256562540937647;
	image_points[1][0] = -19.454109269458844;
	image_points[0][1] = -16.590459545622990;
	image_points[1][1] = -7.996912353622662;
	image_points[0][2] = 7.035000697871075;
	image_points[1][2] = 36.290369665653714;
	image_points[0][3] = 33.078213010634570;
	image_points[1][3] = -8.072373343202950;
	std::chrono::steady_clock::time_point begin;
	std::this_thread::sleep_for(std::chrono::microseconds(2500));
	std::chrono::steady_clock::time_point end;
	begin = std::chrono::steady_clock::now();
	for(int i = 0; i < 100000; i++)
	{
		this->computePosit(image_points);
	}
	end = std::chrono::steady_clock::now();
	this->mutexLog.lock();
	std::cout << "Time difference POSIT = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<std::endl;
	this->mutexLog.unlock();
	image_points[0][0] = 9.256562540937647;
	image_points[1][0] = -19.454109269458844;
	image_points[0][1] = -16.590459545622990;
	image_points[1][1] = -7.996912353622662;
	image_points[0][2] = 7.412045903539938;
	image_points[1][2] = 36.997162557109505;
	image_points[0][3] = 33.242078313261715;
	image_points[1][3] = -7.994080212156028;
	this->computePosit(image_points);*/
	bool tmp_stop = false;
	do {
#if DEBUG == DEBUG_YES
		this->mutexLog.lock();
		std::cout << "Checking Hough Event" << std::endl;
		this->mutexLog.unlock();
#endif
		while(this->m_ev_queue.empty())
		{
			//this->mutexLog.lock();
			//std::cout << "Waiting Hough Event" << std::endl;
			//this->mutexLog.unlock();
			this->m_main_loop_cv.wait(lck);
		}
#if DEBUG == DEBUG_YES
		this->mutexLog.lock();
		std::cout << "Hough Event received" << std::endl;
		this->mutexLog.unlock();
#endif
		this->m_ev_add_mutex.lock();
		HoughEvent e = this->m_ev_queue.front();
		this->m_ev_queue.pop();
		this->m_ev_add_mutex.unlock();
#if DEBUG == DEBUG_YES
		this->mutexLog.lock();
		std::cout << "Interpreting Hough Event" << std::endl;
		this->mutexLog.unlock();
#endif
		switch(e.a)
		{
		case 1:
			this->computeEvent(e.theta,e.dist,e.t, e.line_id);
			break;
		case 2:
			tmp_stop = true;
			break;
		default:
			this->mutexLog.lock();
			std::cout << "Unknown Hough event: " << e.a << std::endl;
			this->mutexLog.unlock();
			break;
		}
	}while(!tmp_stop);
	this->mutexLog.lock();
	std::cout << "Stopped my things ! PNP" << std::endl;
	this->mutexLog.unlock();
}

void PNPThread::computeEvent(double theta, double dist, unsigned int t, int line_id)
{
	if(this->m_nbr_lines_identified == 4)
	{
		if(line_id > 0)
		{
			bool rotated;
			line_id--;
			if(std::abs(theta-this->m_line_parameters[line_id][0]) < std::abs(std::fmod(theta+PI, 2*PI)-this->m_line_parameters[line_id][0]))
			{
				rotated = false;
			}
			else
			{
				rotated = true;
			}
			this->updateLineParameters(theta,dist,rotated,line_id);
			this->computeLineIntersection();
			this->computePosit();
			this->updateFilteringArray();
		}
		else
		{
			this->mutexLog.lock();
			std::cout << "Warning: lines identified but still not line id:" << line_id << std::endl;
			this->mutexLog.unlock();
#if DEBUG == DEBUG_YES
		/*this->mutexLog.lock();
		std::cout << "Warning: lines identified but still not line id." << std::endl;
		this->mutexLog.unlock();*/
#endif
		}
	}
	else if(this->m_nbr_lines_identified > 4)
	{
		this->mutexLog.lock();
		std::cout << "Error: too many lines detected !" << std::endl;
		this->mutexLog.unlock();
	}
	else
	{
		int can_be_a_new_line = 0;
		int candidate_line = -1;
		double dt,dd, best_dt, best_dd, best_dist;
		bool rotated, best_rotated;
		best_dist = 1e300;
		for(int i = 0; i < this->m_nbr_lines_identified; i++)
		{
			dt = std::min(std::abs(theta-this->m_line_parameters[i][0]), std::abs(std::fmod(theta+PI, 2*PI)-this->m_line_parameters[i][0]));
			if(dt == std::abs(theta-this->m_line_parameters[i][0]))
			{
				dd = std::abs(dist-this->m_line_parameters[i][1]);
				rotated = false;
			}
			else
			{
				dd = 2*std::min(dist-this->m_line_parameters[i][1],dist) + std::max(dist-this->m_line_parameters[i][1],dd);
				rotated = true;
			}
			if(dt < PI/6 && sqrt(140.0*dt*dt+dd*dd) < 140)
			{
				if(sqrt(140.0*dt*dt+dd*dd) < best_dist)
				{
					best_dist = sqrt(140.0*dt*dt+dd*dd);
					candidate_line = i;
					best_dt = dt;
					best_dd = dd;
					best_rotated = rotated;
				}
			}
			else
			{
				can_be_a_new_line++;
			}
		}
		if(candidate_line >= 0)
		{
			this->updateLineParameters(theta,dist,best_rotated,candidate_line);
		}
		else if(candidate_line == -1 && can_be_a_new_line == this->m_nbr_lines_identified)
		{
			this->m_line_parameters[this->m_nbr_lines_identified][0] = theta;
			this->m_line_parameters[this->m_nbr_lines_identified][1] = dist;
			this->m_nbr_lines_identified++;
			if(this->m_nbr_lines_identified == 4)
			{
				double xmax, ymin;
				int line_0, line_1, line_2, line_3;
				xmax = -99999;
				ymin = 99999;
				line_0 = -1;
				line_1 = -1;
				line_2 = -1;
				line_3 = -1;
				for(int i = 0; i < this->m_nbr_lines_identified; i++)
				{
					double theta = this->m_line_parameters[i][0];
					double dist = this->m_line_parameters[i][1];
					if(cos(theta)/sin(theta) < 0)
					{
						if(cos(theta)/sin(theta) < ymin)
						{
							if(line_0 > -1)
							{
								line_1 = line_0;
							}
							line_0 = i;
							ymin = cos(theta)/sin(theta);
						}
						else
						{
							line_1 = i;
						}
					}
					else
					{
						if(cos(theta)/sin(theta) > xmax)
						{
							if(line_3 > -1)
							{
								line_2 = line_3;
							}
							line_3 = i;
							xmax = cos(theta)/sin(theta);
						}
						else
						{
							line_2 = i;
						}
					}
				}
				double** tmp_params = new double*[this->m_nbr_lines_identified];
				for(int i = 0; i < this->m_nbr_lines_identified; i++)
				{
					tmp_params[i] = new double[2];
					tmp_params[i][0] = this->m_line_parameters[i][0];
					tmp_params[i][1] = this->m_line_parameters[i][1];
				}
				line_0 = -1;
				if(line_0 == -1 || line_1 == -1 || line_2 == -1 || line_3 == -1 )
				{
					this->m_nbr_lines_identified = 0;
#if DEBUG == DEBUG_YES
					this->mutexLog.lock();
					std::cout << "Warning: Reseting the lines because it could not detected it" << std::endl;
					this->mutexLog.unlock();
#endif
				}
				else
				{
					this->m_line_parameters[0][0] = tmp_params[line_0][0];
					this->m_line_parameters[0][1] = tmp_params[line_0][1];
					this->m_line_parameters[1][0] = tmp_params[line_1][0];
					this->m_line_parameters[1][1] = tmp_params[line_1][1];
					this->m_line_parameters[2][0] = tmp_params[line_2][0];
					this->m_line_parameters[2][1] = tmp_params[line_2][1];
					this->m_line_parameters[3][0] = tmp_params[line_3][0];
					this->m_line_parameters[3][1] = tmp_params[line_3][1];
#if DEBUG == DEBUG_YES
					this->mutexLog.lock();
					std::cout << "Detected lines:" << std::endl;
					for(int i = 0; i < 4; i++)
					{
						std::cout << "Line " << i << ": Theta=" << this->m_line_parameters[i][0] << " Dist=" << this->m_line_parameters[i][1] << std::endl;
					}
					this->mutexLog.unlock();
#endif
					this->computeLineIntersection();
					this->computePosit();
					this->updateFilteringArray();
					this->m_web_string_stream << "{\"tracking\": true},";
					this->m_ht->activateTracking();
				}
			}
		}

	}
}

void PNPThread::computePosit()
{
	double** image_points = this->m_line_inters;
	// COMPUTE CORRECTION
	double** correction;
	correction = new double*[2];
	for(int i = 0; i < 2; i++)
	{
		correction[i] = new double[4];
	}
	double** ones1_2;
	ones1_2 = new double*[2];
	for(int i = 0; i < 2; i++)
	{
		ones1_2[i] = new double[1];
	}
	ones1_2[0][0] = 1.0;
	ones1_2[1][0] = 1.0;
	//dispMat(m_epsilon,4,1);
	//dispMat(ones1_2,1,2);
	multMat(m_epsilon,ones1_2,correction,4,1,2);
	//dispMat(correction,4,2);
	// COMPUTE NEW POINTS X and XP
	double** x = new double*[2];
	double** xp = new double*[2];
	for(int i = 0; i < 2; i++)
	{
		x[i] = new double[4];
		xp[i] = new double[4];
	}
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			x[j][i] = (correction[j][i]+1)*image_points[j][i];
		}
		for(int j = 0; j < 2; j++)
		{
			xp[j][i] = x[j][i]-x[j][0];
		}
	}
	//dispMat(xp,4,2);
	// COMPUTE IJ
	double** IJt = new double*[2];
	for(int i = 0; i < 2; i++)
	{
		IJt[i] = new double[3];
	}
	double** IJ = new double*[3];
	for(int i = 0; i < 3; i++)
	{
		IJ[i] = new double[2];
	}
	multMat(this->m_object_matrix, xp, IJt, 3, 4, 2);
	//dispMat(IJt,3,2);
	transMat(IJt,IJ,3,2);
	//dispMat(IJ,2,3);
	double nI = 0.0;
	double nJ = 0.0;
	for(int i = 0; i < 3; i++)
	{
		nI += IJ[i][0]*IJ[i][0];
		nJ += IJ[i][1]*IJ[i][1];
	}
	nI = sqrt(nI);
	nJ = sqrt(nJ);
	for(int i = 0; i < 3; i++)
	{
		IJ[i][0] = IJ[i][0]/nI;
		IJ[i][1] = IJ[i][1]/nJ;
	}
	//this->mutexLog.lock();
	//std::cout << "Normes: " << nI << " " << nJ << std::endl;
	//this->mutexLog.unlock();
	//dispMat(IJ,2,3);
	// COMPUTE K
	double** k = new double*[3];
	for(int i = 0; i<3; i++)
	{
		k[i] = new double[1];
		k[i][0] = IJ[(i+1)%3][0]*IJ[(i+2)%3][1]-IJ[(i+1)%3][1]*IJ[(i+2)%3][0];
		//std::cout << IJ[(i+1)%3][0] << "*" << IJ[(i+2)%3][1] << "-" << IJ[(i+1)%3][1] <<"*" << IJ[(i+2)%3][0] << std::endl;
	}
	//dispMat(k, 1, 3);
	double** kt = new double*[1];
	kt[0] = new double[3];
	transMat(k,kt,1,3);
	//COMPUTE TRANSLATIONS
	double Z0 = 2.0*this->m_focal_length/(nI+nJ);
	double Tz = Z0;
	//std::cout << "Tz " << Tz << std::endl;
	double Tx = image_points[0][0]*Z0/this->m_focal_length;
	//std::cout << "Tx " << Tx << std::endl;
	double Ty = image_points[1][0]*Z0/this->m_focal_length;
	//std::cout << "Ty " << Ty << std::endl;
	//COMPUTE EPSILON
	double** tmp_eps = new double*[1];
	tmp_eps[0] = new double[4];
	//dispMat(this->m_object_points,4,3);
	multMat(this->m_object_points,kt,tmp_eps,4,3,1);
	//dispMat(tmp_eps,4,1);
	for(int i = 0; i < 4; i++)
	{
		this->m_epsilon[0][i] = tmp_eps[0][i]/Z0;
	}
	//dispMat(this->m_epsilon,4,1);
}

void PNPThread::addEvent(double theta, double dist, unsigned int t, int line_id)
{
	this->m_ev_add_mutex.lock();
	this->m_ev_queue.push(HoughEvent(theta,dist,t,line_id,1));
	this->m_ev_add_mutex.unlock();
	this->m_main_loop_cv.notify_all();
}

void PNPThread::multMat(double** m1, double** m2, double** res, int ligne, int inter, int colonne)
{
	for(int i = 0; i<ligne; i++)
	{
		for(int j = 0; j<colonne; j++)
		{
			res[j][i] = 0.0;
			for(int k = 0; k<inter; k++)
			{
				res[j][i] += m1[k][i]*m2[j][k];
				//std::cout << m1[k][i] <<"*" << m2[j][k] << "+";
			}
			//std::cout << "=" << res[j][i] << std::endl;
		}
	}
}

void PNPThread::stop()
{
	this->m_ev_add_mutex.lock();
	this->m_ev_queue.push(HoughEvent(0,0,0,0,2));
	this->m_ev_add_mutex.unlock();
	BaseThread::stop();
}

void PNPThread::dispMat(double** matrix, int ligne, int colonne)
{
	this->mutexLog.lock();
	std::cout << "Matrix: " << std::endl;
	for(int i = 0; i < ligne; i++)
	{
		std::cout << "L" << i << ": ";
		for(int j = 0; j < colonne; j++)
		{
			std::cout << matrix[j][i] << "\t";
		}
		std::cout << std::endl;
	}
	this->mutexLog.unlock();
}

void PNPThread::transMat(double** matrix, double** res, int ligne, int colonne)
{
	for(int i = 0; i < ligne; i++)
	{
		for(int j = 0; j < colonne; j++)
		{
			res[i][j] = matrix[j][i];
		}
	}
}

void PNPThread::updateFilteringArray()
{
	int zone_x = 10;//this->m_ht->getZoneX();
	int zone_y = 10;//this->m_ht->getZoneY();
	//printFilteringMap();
	for(int i = 0; i < 4; i++)
	{
#if DEBUG >= DEBUG_HARD
		this->mutexLog.lock();
		std::cout << "Computing indexes" << std::endl;
		this->mutexLog.unlock();
#endif
		int rho_index = (int)round(this->m_line_parameters[i][1]/this->m_ht_rho_max*(double)(this->m_ht_map_y));
		int theta_index = (int)round(this->m_line_parameters[i][0]*this->m_ht_map_x/2.0/PI);
#if DEBUG == DEBUG_YES
		this->mutexLog.lock();
		std::cout << "Filtering array: rho_i=" << rho_index << " theta_i=" << theta_index << std::endl;
		this->mutexLog.unlock();
#endif
		if(this->m_current_filter_centers[i][0] != theta_index && this->m_current_filter_centers[i][1] != rho_index)
		{
			this->m_filter_mutex.lock();
			for(int j = -zone_x; j <= zone_x; j++)
			{
				for(int k = -zone_y; k <= zone_y; k++)
				{
					if(this->m_current_filter_centers[i][1]+k >= 0)
					{
#if DEBUG >= DEBUG_HARD
						this->mutexLog.lock();
						std::cout << "Filtering array reset: rho_i=" << std::min(this->m_current_filter_centers[i][1]+k,this->m_ht_map_y) << " theta_i=" << (this->m_current_filter_centers[i][0]+j+this->m_ht_map_x)%this->m_ht_map_x << std::endl;
						this->mutexLog.unlock();
#endif
						this->m_filtering_array[(this->m_current_filter_centers[i][0]+j+this->m_ht_map_x)%this->m_ht_map_x][std::min(this->m_current_filter_centers[i][1]+k,this->m_ht_map_y)] = 0;
					}
					else
					{
#if DEBUG >= DEBUG_HARD
						this->mutexLog.lock();
						std::cout << "Filtering array reset reverse: rho_i=" << (-(this->m_current_filter_centers[i][1]+k)-1) << " theta_i=" << (this->m_current_filter_centers[i][0]+j+(this->m_ht_map_x >> 1))%this->m_ht_map_x << std::endl;
						this->mutexLog.unlock();
#endif
						this->m_filtering_array[(this->m_current_filter_centers[i][0]+j+(this->m_ht_map_x >> 1))%this->m_ht_map_x][(-(this->m_current_filter_centers[i][1]+k)-1)] = 0;
					}
				}
			}
			this->m_filter_mutex.unlock();
			this->m_current_filter_centers[i][0] = theta_index;
			this->m_current_filter_centers[i][1] = rho_index;
			this->m_filter_mutex.lock();
			for(int j = -zone_x; j < zone_x; j++)
			{
				for(int k = -zone_y; k < zone_y; k++)
				{
					if(this->m_current_filter_centers[i][1]+k >= 0)
					{
#if DEBUG >= DEBUG_HARD
						this->mutexLog.lock();
						std::cout << "Filtering array set: rho_i=" << std::min(this->m_current_filter_centers[i][1]+k,this->m_ht_map_y) << " theta_i=" << (this->m_current_filter_centers[i][0]+j+this->m_ht_map_x)%this->m_ht_map_x << std::endl;
						this->mutexLog.unlock();
#endif
						this->m_filtering_array[(this->m_current_filter_centers[i][0]+j+this->m_ht_map_x)%this->m_ht_map_x][std::min(this->m_current_filter_centers[i][1]+k,this->m_ht_map_y)] = i+1;
					}
					else
					{
#if DEBUG >= DEBUG_HARD
						this->mutexLog.lock();
						std::cout << "Filtering array set reverse: rho_i=" << (-(this->m_current_filter_centers[i][1]+k)-1) << " theta_i=" << (this->m_current_filter_centers[i][0]+j+(this->m_ht_map_x >> 1))%this->m_ht_map_x << std::endl;
						this->mutexLog.unlock();
#endif
						this->m_filtering_array[(this->m_current_filter_centers[i][0]+j+(this->m_ht_map_x >> 1))%this->m_ht_map_x][(-(this->m_current_filter_centers[i][1]+k)-1)] = i+1;
					}
				}
			}
			this->m_filter_mutex.unlock();
		}
	}
	//printFilteringMap();
}

void PNPThread::updateLineParameters(double theta, double dist, bool rotated, int line_id)
{
	if(rotated)
	{
		this->m_line_parameters[line_id][1] = -dist*this->m_confidence_coef+this->m_line_parameters[line_id][1]*(1.0-this->m_confidence_coef);
		if(this->m_line_parameters[line_id][1] < 0)
		{
			this->m_line_parameters[line_id][0] = theta*this->m_confidence_coef+std::fmod(this->m_line_parameters[line_id][0]+PI, 2*PI)*(1.0-this->m_confidence_coef);
			this->m_line_parameters[line_id][1] = -this->m_line_parameters[line_id][1];
		}
		else
		{
			this->m_line_parameters[line_id][0] = std::fmod(theta+PI, 2*PI)*this->m_confidence_coef+this->m_line_parameters[line_id][0]*(1.0-this->m_confidence_coef);
		}
	}
	else
	{
		this->m_line_parameters[line_id][0] = theta*this->m_confidence_coef+this->m_line_parameters[line_id][0]*(1.0-this->m_confidence_coef);
		this->m_line_parameters[line_id][1] = dist*this->m_confidence_coef+this->m_line_parameters[line_id][1]*(1.0-this->m_confidence_coef);
	}
}

void PNPThread::computeLineIntersection()
{
	if(this->m_nbr_lines_identified != 4)
	{
		this->mutexLog.lock();
		std::cout << "Error: not enough line, can not compute the line intersection" << std::endl;
		this->mutexLog.unlock();
	}
	else
	{
		double t1,t2,d1,d2,x,y;
		this->m_web_mutex.lock();
		for(int i = 0; i < 4; i++)
		{
			t1 = this->m_line_parameters[(i+1)%4][0];
			t2 = this->m_line_parameters[(i+2)%4][0];
			d1 = this->m_line_parameters[(i+1)%4][1];
			d2 = this->m_line_parameters[(i+2)%4][1];
			y = (d1*cos(t2)-d2*cos(t1))/(sin(t1)*cos(t2)-sin(t2)*cos(t1));
            x = (d1*sin(t2)-d2*sin(t1))/(cos(t1)*sin(t2)-cos(t2)*sin(t1));
    		this->m_line_inters[i][0] = x;
    		this->m_line_inters[i][1] = y;
		}
		this->m_web_mutex.unlock();
#if DEBUG == DEBUG_YES
		std::cout << "Detected intersections:" << std::endl;
#endif
		//this->m_web_mutex.lock();
		//this->m_web_string_stream << "{\"intersection\": [";
		//for(int i = 0; i < 4; i++)
		//{
#if DEBUG == DEBUG_YES
			//std::cout << "Inter " << i << ": X=" << this->m_line_inters[i][0] << " Y=" << this->m_line_inters[i][1] << std::endl;
#endif
			//this->m_web_string_stream << "{\"x\":" << this->m_line_inters[i][0] << ",\"y\":" << this->m_line_inters[i][1] << "},";
		//}
		//this->m_web_string_stream << "{}]},";
		//this->m_web_mutex.unlock();
	}
}

int PNPThread::getFilterValue(int t, int d)
{
	this->m_filter_mutex.lock();
	int value = this->m_filtering_array[t][d];
	this->m_filter_mutex.unlock();
	return value;
}

void PNPThread::printFilteringMap()
{
	this->mutexLog.lock();
	for(int i = 0; i < this->m_ht_map_y; i++)
	{
		std::cout << i << ": " ;
		for(int j = 0; j < this->m_ht_map_x; j++)
		{
			std::cout << this->getFilterValue(j,i) << " ";
		}
		std::cout << std::endl;
	}
	this->mutexLog.unlock();
}

std::string PNPThread::generateWebServerData()
{
	this->m_web_mutex.lock();
	this->m_web_string_stream << "{\"intersection\": [";
	for(int i = 0; i < 4; i++)
	{
		this->m_web_string_stream << "{\"x\":" << this->m_line_inters[i][0] << ",\"y\":" << this->m_line_inters[i][1] << "},";
	}
	this->m_web_string_stream << "{}]},";
	this->m_web_string_stream << "{\"end\":1}]";
	std::string tmp = this->m_web_string_stream.str();
	this->m_web_string_stream.str("");
	//std::cout << "Data generated: " << tmp << std::endl;
	this->m_web_string_stream << "[";
	this->m_web_mutex.unlock();
	return tmp;
	//return "Test";
}
