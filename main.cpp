// required by EXIT_SUCCESS, EXIT_FAILURE
#include <cstdlib>

// required by std::cout
#include <iostream>

// required by crowbox::EXCEPTION
#include "exception.h"

#include "circullar-buffer.h"

//#include "lockeless-queue.h"

// required by test_auto_thread()
#include "auto-thread-example-impl.h"

#include "auto-thread-unit-test.h"

#include "graph.h"
#include "allocator-fixed-size.h"



class K {
public:
	int get_id() const {
		return 0;
	}


	bool operator < (const K & other_k) const {
		return (this->m_id < other_k.m_id);
	}

private:

	int m_id;
};


namespace std {
	template <>
	struct hash<K> {
		size_t operator () (const K & k) const {
			return std::hash<int>()(k.get_id());
		}
	};
}


int main() {
	test_auto_thread();
	auto_thread_unit_test();
	try {
		throw crowbox::EXCEPTION("AAA");
	}
	catch (const std::exception & err) {
		std::cout << err.what() << std::endl;
		return EXIT_FAILURE;
	}

	crowbox::Graph<int> g;
	return EXIT_SUCCESS;
}
