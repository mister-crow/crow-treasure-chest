#pragma once

// required by std::atomic_flag
#include <atomic>

// required by std::mutex
#include <mutex>


namespace crowbox {


class CombinedLock {
public:
	explicit CombinedLock(unsigned spin_count_before_lock = 10000) :
		m_mutex(),
		m_lock(false),
		m_spin_count_before_lock(spin_count_before_lock) {
	}

	void lock() {
		unsigned remaining_spin_count = m_spin_count_before_lock;
		while (remaining_spin_count) {
			if (!m_lock.test_and_set()) {
				break;
			}
			--remaining_spin_count;
		}
		m_mutex.lock();
	}

	void unlock() {
		m_lock.clear();
		m_mutex.unlock();
	}
 
private:
	std::mutex m_mutex;
	std::atomic_flag m_lock;
	const unsigned m_spin_count_before_lock;
};

}
