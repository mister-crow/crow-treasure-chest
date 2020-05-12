#pragma once

#include "auto-thread.h"


namespace crowbox {


template <class Runnable>
std::shared_ptr<AutoThread<Runnable>> AutoThread<Runnable>::create(
	std::shared_ptr<Runnable> runnable_ptr,
	RunnableMethodPtrType method_ptr)
{
	return std::make_shared<AutoThread<Runnable>>(runnable_ptr, method_ptr);
}


template <class Runnable>
AutoThread<Runnable>::AutoThread(
        std::shared_ptr<Runnable> runnable_ptr,
		RunnableMethodPtrType method_ptr) :
    m_runnable_method_ptr(method_ptr),
    m_runnable_object_ptr(runnable_ptr),
    m_thread_ptr(),
    m_thread_active_mutex(),
    m_sync_cond_var(),
    m_sync_mutex(),
    m_referenced_acquired(false) {
}

template <class Runnable>
void AutoThread<Runnable>::start() {
    std::unique_lock<std::mutex> auto_mutex_lock(m_sync_mutex);
    m_thread_ptr = std::make_unique<std::thread>(
		&AutoThread<Runnable>::thread_method, this);
    while (!m_referenced_acquired.load()) {
        m_sync_cond_var.wait(auto_mutex_lock);
    }
}

template <class Runnable>
void AutoThread<Runnable>::thread_method() {
    m_sync_mutex.lock();
    {
        std::shared_ptr<AutoThread> protector_ref = this->shared_from_this();
        m_referenced_acquired.store(true);
        m_sync_cond_var.notify_one();
        {
            std::lock_guard<std::timed_mutex> t_lock(m_thread_active_mutex);
            m_sync_mutex.unlock();
            Runnable* runnable_object_ptr = m_runnable_object_ptr.get();
            (runnable_object_ptr->*m_runnable_method_ptr)();
        }
		// thread needs to be detached in case of exception
        m_thread_ptr->detach();
    }
}

template <class Runnable>
void AutoThread<Runnable>::wait() {
    if (!m_referenced_acquired.load()) {
        return;
    }
    std::lock_guard<std::timed_mutex> mutex_auto_lock(m_thread_active_mutex);
    return;
}

template <class Runnable>
template <class Rep, class Period>
bool AutoThread<Runnable>::wait_for(
	const std::chrono::duration<Rep, Period> & timout_duration)
{
    if (!m_referenced_acquired.load()) {
        return false;
    }

	std::unique_lock<std::timed_mutex> scoped_locker(
		m_thread_active_mutex,
		std::defer_lock);

	if (scoped_locker.try_lock_for(timout_duration)) {
		return true;
	}
	else {
		return false;
	}
}

}
