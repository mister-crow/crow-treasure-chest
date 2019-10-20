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
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		case TestMode::return_w_delay:
			return;
		default:
			return;
		}
	}

private:
	TestMode m_mode;
};


bool test_scenario_1() {
	using TestMode = RunnableTester::TestMode;
	using crowbox::AutoThread;
	auto runnable_tester_ptr = std::make_shared<RunnableTester>(TestMode::instant_return);
	auto auto_thread_ptr = AutoThread<RunnableTester>::create(
		runnable_tester_ptr, &RunnableTester::do_something);
	auto_thread_ptr->start();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	auto_thread_ptr->wait();
	return true;
}

bool test_scenario_2() {
	using TestMode = RunnableTester::TestMode;
	using crowbox::AutoThread;
	auto runnable_tester_ptr = std::make_shared<RunnableTester>(TestMode::return_w_delay);
	{
		auto auto_thread_ptr = AutoThread<RunnableTester>::create(
			runnable_tester_ptr, &RunnableTester::do_something);
		auto_thread_ptr->start();
	}
	return true;
}


bool auto_thread_unit_tests() {
	test_scenario_1();
	test_scenario_2();
	return false;
}
