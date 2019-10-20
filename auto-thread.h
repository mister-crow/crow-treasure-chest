#pragma once

// required by std::atomic
#include <atomic>

// required by std::condition_variable
#include <condition_variable>

// required by std::enable_shared_from_this, std::unique_ptr, std::shared_ptr
#include <memory>

// required by std::mutex, std::timed_mutex
#include <mutex>

// required by std::thread
#include <thread>

// required by std::chrono::duration
#include <chrono>


namespace crowbox {

// The class is similar to std::thread but with additional self protecting
// features. Each instance of AutoThread is wrapped into shared_ptr wich means
// that each reference to AutoThread is bound to reference counter.
// This implementation makes sure that in case if client suddenly looses the last
// reference AutoThread the active thread itself maintaince additional reference
// to itself to make sure that destructor will not be called on objects that
// are in use by the active thread.
template <class Runnable>
class AutoThread : public std::enable_shared_from_this<AutoThread<Runnable>> {
public:
    typedef void (Runnable::*RunnableMethodPtrType)();

	// Instance of AutoThread should always be wrapped into shared_ptr, it is a
	// rule enforced by std::enable_shared_from_this. There is a reason to
	// restrict access to the constructor and use dedicated function for object
	// creation
	static std::shared_ptr<AutoThread<Runnable>> create(
			std::shared_ptr<Runnable> runnable_ptr,
			RunnableMethodPtrType method_ptr);

    virtual ~AutoThread() = default;

	// Start thread
    void start();

	// wait for thread exit
    void wait();

	template <class Rep, class Period>
	bool wait_for(const std::chrono::duration<Rep, Period> & timout_duration);

	// it is illegal to copy or move this kind of object because object members
	// are shared with active thread
    AutoThread(const AutoThread&) = delete;
	AutoThread(const AutoThread&&) = delete;
    AutoThread& operator = (const AutoThread&) = delete;
    AutoThread& operator = (const AutoThread&&) = delete;

	// TODO: make this private and make sure std::make_shared can access it
    AutoThread(std::shared_ptr<Runnable> runnable_ptr, RunnableMethodPtrType method_ptr);
private:

    void thread_method();

    RunnableMethodPtrType m_runnable_method_ptr;

    std::shared_ptr<Runnable> m_runnable_object_ptr;
    std::unique_ptr<std::thread> m_thread_ptr;
    std::timed_mutex m_thread_active_mutex;
    std::condition_variable m_sync_cond_var;
    std::mutex m_sync_mutex;
    std::atomic<bool> m_referenced_acquired;
};


}
