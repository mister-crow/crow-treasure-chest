// required by std::enable_shared_from_this, std::make_shared
#include <memory>


#include "auto-thread.h"
#include "auto-thread-impl.h"


class RunnableSample {
public:
	void do_something() {
	}
};


void test_auto_thread() {
	using crowbox::AutoThread;
	std::shared_ptr<RunnableSample> runnable_ptr = std::make_shared<RunnableSample>();
	auto auto_thread_ptr = AutoThread<RunnableSample>::create(runnable_ptr, &RunnableSample::do_something);

	auto_thread_ptr->start();
	auto_thread_ptr->wait();

}
