#include "uart_thread.h"

UARTThread::UARTThread() {
	this->m_ht = 0;
	this->m_fd = -2;/*
#if OS == WINDOWS
	this->m_fd = RS232_OpenComport(7,12000000,"8N1",1);
#elif OF == LINUX
	this->m_fd = RS232_OpenComport(16,12000000,"8N1",1);
#endif
	this->mutexLog.lock();
	std::cout << "File descriptor: " << this->m_fd << std::endl;
	this->mutexLog.unlock();
	RS232_CloseComport(this->m_fd);/*
	/*libusb_device **devs;
	int r;
	ssize_t cnt;

	this->mutexLog.lock();
	std::cout << "Init: libusb" << std::endl;
	this->mutexLog.unlock();

	r = libusb_init(NULL);
	if (r < 0)
	{
		this->mutexLog.lock();
		std::cout << "Error: libusb initialization" << std::endl;
		this->mutexLog.unlock();
	}
	else
	{
		cnt = libusb_get_device_list(NULL, &devs);
		if (cnt < 0){
			libusb_exit(NULL);
			this->mutexLog.lock();
			std::cout << "Error: libusb device list " << cnt << std::endl;
			this->mutexLog.unlock();
		}
		else
		{
			libusb_device *dev;
			int i = 0, j = 0;
			uint8_t path[8];

			while ((dev = devs[i++]) != NULL) {
				struct libusb_device_descriptor desc;
				int r = libusb_get_device_descriptor(dev, &desc);
				if (r < 0) {
					fprintf(stderr, "failed to get device descriptor");
					return;
				}

				printf("%04x:%04x (bus %d, device %d)",
					desc.idVendor, desc.idProduct,
					libusb_get_bus_number(dev), libusb_get_device_address(dev));

				r = libusb_get_port_numbers(dev, path, sizeof(path));
				if (r > 0) {
					printf(" path: %d", path[0]);
					for (j = 1; j < r; j++)
						printf(".%d", path[j]);
				}
				printf("\n");
			}

			libusb_free_device_list(devs, 1);

			libusb_exit(NULL);
		}
	}*/

	std::cout << "Init ended: RS232" << std::endl;
}

UARTThread::~UARTThread() {

}

void UARTThread::threadFunction() {
	this->m_is_launched = true;
	this->mutexLog.lock();
	std::cout << "Doing my things ! UART" << std::endl;
	this->mutexLog.unlock();
//	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
//	for(int i = 0; i < 100000; i++)
//	{
//		this->m_ht->addEvent(112,8,true,1);
//	}
#if MODE == MODE_OFFLINE
	std::ifstream event_file("./tetra_move_y_x_r.csv");
	if(!event_file.is_open())
	{
		this->mutexLog.lock();
		std::cout << "Error: Event file not opened" << std::endl;
		this->mutexLog.unlock();
	}
	else
	{
		std::string tmp_input_line;
		std::string::size_type sz;
		std::getline(event_file, tmp_input_line); //Remove header of the CSV
		int x,y,p,t;
		while (std::getline(event_file, tmp_input_line)) {
			x = std::stoi(tmp_input_line, &sz);
			tmp_input_line = tmp_input_line.substr(sz+1);
			y = std::stoi(tmp_input_line, &sz);
			tmp_input_line = tmp_input_line.substr(sz+1);
			p = std::stoi(tmp_input_line, &sz);
			tmp_input_line = tmp_input_line.substr(sz+1);
			t = std::stoi(tmp_input_line);
			this->m_ht->addEvent(x,y,p==1,t);
		}
	}
#endif
//	std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
	this->mutexLog.lock();
//	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() <<std::endl;
	std::cout << "Stopped my things ! UART" << std::endl;
	this->mutexLog.unlock();
}

void UARTThread::setHoughThread(HoughThread* ht)
{
	this->m_ht = ht;
}
