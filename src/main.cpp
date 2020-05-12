// required by EXIT_SUCCESS, EXIT_FAILURE
#include <cstdlib>

// required by std::cerr, std::cout
#include <iostream>

// required by crowbox::EXCEPTION
#include "exception.h"

//#include "ring-buffer.h"

//#include "lockeless-queue.h"

// required by test_auto_thread()
#include "auto-thread-example-impl.h"

// required by auto_thread_unit_tests()
#include "auto-thread-unit-test.h"

// required by state_machine_unit_tests()
#include "state-machine-unit-test.h"

// required by Graph
//#include "graph.h"

// required by intervals_unit_test()
#include "intervals-unit-test.h"


int main() {
	test_auto_thread();

	bool has_fail = false;

	if (!auto_thread_unit_tests()) {
		std::cerr << "Auto Thread tests failed" << std::endl;
		has_fail = true;
	}

	if (!state_machine_unit_tests()){
		std::cerr << "State Machine tests failed" << std::endl;
		has_fail = true;
	}

	if (!intervals_unit_test()) {
		std::cerr << "Interval tests failed" << std::endl;
		has_fail = true;
	}

	if (has_fail) {
		return EXIT_FAILURE;
	}

	std::cout << "All tests pass" << std::endl;
	return EXIT_SUCCESS;
}
