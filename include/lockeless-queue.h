#pragma once

// required by CombinedLock
#include "combined-lock.h"


namespace crowbox {
namespace lockeless_queue {


class DataBlock {
public:
	DataBlock * m_next_ptr = nullptr;
	int m_data = 0;
};


class Producer {
	void push_data(void * pointer, size_t size);
};

class Consumer {
	void pull_data();
};


class Item {
public:
	Item * m_next_ptr = nullptr;
};



// Single Producer and Single Consumer
template <class T>
class LockelessQueue {
public:
	void push_front(const T & item) {
		if (m_first_ptr) {
			if (m_last_ptr != m_first_ptr) {
			}
			else {
			}
		}
	}

	void pop_back(T & item) {
		if (m_last_ptr) {
			if (m_last_ptr != m_first_ptr) {
			}
			else {
			}
		}
	}

private:
	CombinedLock m_lock;
	Item * m_first_ptr = nullptr;
	Item * m_last_ptr = nullptr;
};


}
}
