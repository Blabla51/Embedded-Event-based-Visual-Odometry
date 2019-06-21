#include "pnp_thread.h"

PNPThread::PNPThread() {

}

PNPThread::~PNPThread() {

}

void PNPThread::threadFunction() {
	this->mutexLog.lock();
	std::cout << "Doing my things ! PNP" << std::endl;
	this->mutexLog.unlock();
	this->mutexLog.lock();
	std::cout << "Stopped my things ! PNP" << std::endl;
	this->mutexLog.unlock();
}
