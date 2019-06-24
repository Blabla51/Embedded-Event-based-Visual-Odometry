#include "pnp_thread.h"
#include "hough_thread.h"

PNPThread::PNPThread(double fl) {
	this->m_ht = 0;
	this->m_focal_length = fl;

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
}

PNPThread::~PNPThread() {

}

void PNPThread::threadFunction() {
	this->m_is_launched = true;
	this->mutexLog.lock();
	std::cout << "Doing my things ! PNP" << std::endl;
	this->mutexLog.unlock();
	std::unique_lock<std::mutex> lck(this->m_main_loop_mutex);
	double** image_points = new double*[2];
	for(int i = 0; i < 4; i++)
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
	for(int i = 0; i < 1000000; i++)
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
	this->computePosit(image_points);
	bool tmp_stop = false;
	do {
		while(this->m_ev_queue.empty())
		{
			this->mutexLog.lock();
			std::cout << "Waiting event " << std::endl;
			this->mutexLog.unlock();
			this->m_main_loop_cv.wait(lck);
		}
		this->m_ev_add_mutex.lock();
		HoughEvent e = this->m_ev_queue.front();
		this->m_ev_queue.pop();
		this->m_ev_add_mutex.unlock();
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
		}
	}while(!tmp_stop);
	this->mutexLog.lock();
	std::cout << "Stopped my things ! PNP" << std::endl;
	this->mutexLog.unlock();
}

void PNPThread::computeEvent(double theta, double dist, unsigned int t, int line_id)
{

}

void PNPThread::computePosit(double** image_points)
{
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

void PNPThread::setHoughThread(HoughThread* ht)
{
	this->m_ht = ht;
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
