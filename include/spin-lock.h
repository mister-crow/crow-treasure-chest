#pragma once

// required by std::atomic_flag
#include <atomic>


namespace crowbox {

/// Implementation of spin lock using std::atomic_flag.
class SpinLock {
public:

	SpinLock() {
		m_lock.clear();
	}

	bool try_lock() {
		if (!m_lock.test_and_set()) {
			return true;
		}
		else {
			return false;
		}
	}

	void lock() {
		while (!try_lock());
	}

	void unlock() {
		m_lock.clear();
	}

private:
	std::atomic_flag m_lock;
};

}
