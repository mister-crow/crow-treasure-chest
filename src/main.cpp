// required by EXIT_SUCCESS, EXIT_FAILURE
#include <cstdlib>

// required by std::cout
#include <iostream>

// required by crowbox::EXCEPTION
#include "exception.h"

//#include "circullar-buffer.h"

//#include "lockeless-queue.h"

// required by test_auto_thread()
#include "auto-thread-example-impl.h"

// required by auto_thread_unit_tests()
#include "auto-thread-unit-test.h"

// required by automata_unit_tests()
#include "automata-unit-test.h"

// required by Graph
#include "graph.h"


void graph_test() {
	typedef int NodeIdType;
	crowbox::Graph<NodeIdType> g;

	g.add_node(10);
}


int main() {
	test_auto_thread();
	auto_thread_unit_tests();
	automata_unit_tests();

	try {
		throw crowbox::EXCEPTION("AAA");
	}
	catch (const std::exception & err) {
		std::cout << err.what() << std::endl;
		return EXIT_FAILURE;
	}

	//crowbox::Graph<int> g;
	return EXIT_SUCCESS;
}
