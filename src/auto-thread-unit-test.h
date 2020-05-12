// required by std::async
#include <future>

// required by std::make_shared
#include <memory>

// required by std::this_thread::sleep_for
#include <thread>

// required std::chrono::milliseconds
#include <chrono>

#include "auto-thread.h"

#include "auto-thread-impl.h"


class RunnableTester {
public:
	enum TestMode {
		instant_return = 1,
		return_w_delay = 2
	};

	explicit RunnableTester(TestMode mode) : m_mode(mode) {
	}

	void do_something() {
		switch (m_mode) {
		case TestMode::instant_return:
			return;
		case TestMode::return_w_delay:
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		default:
			return;
		}
	}

private:
	TestMode m_mode;
};


void test_scenario_1() {
	using TestMode = RunnableTester::TestMode;
	using crowbox::AutoThread;
	auto runnable_tester_ptr = std::make_shared<RunnableTester>(TestMode::instant_return);
	auto auto_thread_ptr = AutoThread<RunnableTester>::create(
		runnable_tester_ptr, &RunnableTester::do_something);
	auto_thread_ptr->start();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	auto_thread_ptr->wait();
}

void test_scenario_2() {
	using TestMode = RunnableTester::TestMode;
	using crowbox::AutoThread;
	auto runnable_tester_ptr = std::make_shared<RunnableTester>(TestMode::return_w_delay);
	{
		auto auto_thread_ptr = AutoThread<RunnableTester>::create(
			runnable_tester_ptr, &RunnableTester::do_something);
		auto_thread_ptr->start();
	}
}

bool auto_thread_unit_tests() {
	{
		auto async_run_result = std::async(test_scenario_1);
		std::future_status status =
			async_run_result.wait_for(std::chrono::milliseconds(300));
		if (status == std::future_status::timeout) {
			return false;
		}
	}
	{
		auto async_run_result = std::async(test_scenario_2);
		std::future_status status =
			async_run_result.wait_for(std::chrono::milliseconds(1));
		if (status == std::future_status::timeout) {
			return false;
		}
	}
	return true;
}
