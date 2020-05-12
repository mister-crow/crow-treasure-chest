#pragma once

// required by std::string
#include <string>

// required by StringNumberCheck
#include "state-machine-use-sample.h"


// This unit test is based on StringNumberCheck sample
template <class StateTransition>
class StringNumberCheckUnitTest {
public:
	bool test(const std::string & number_str, bool expected_answer) {
		bool result = m_num_checker.is_number(number_str);
		if (result != expected_answer) {
			std::cerr << "StringNumberCheck failed on \""
				<< number_str << "\" input\n";
			this->m_all_passed = false;
			return false;
		}
		return true;
	}

	bool get_all_passed() const {
		return m_all_passed;
	}

private:
	bool m_all_passed = true;
	StringNumberCheck<StateTransition> m_num_checker;
};


template <class StateTransition>
bool state_machine_unit_tests_template() {
	StringNumberCheckUnitTest<StateTransition> unit_test;

	unit_test.test("124e52", true);
	unit_test.test("0.124.52", false);
	unit_test.test("124.52", true);
	unit_test.test("124..52", false);
	unit_test.test(".52", true);
	unit_test.test("1111.", true);

	bool all_pass = unit_test.get_all_passed();
	return all_pass;
}

bool state_machine_unit_tests() {
	bool allpass = true;

	if (!state_machine_unit_tests_template<crowbox::FastTransition>() ) {
		std::cerr << "State Machine test failed with FastTransition parameter"
			<< std::endl;
		allpass = false;
	}

	if (!state_machine_unit_tests_template<crowbox::RangedTransition>()) {
		std::cerr << "State Machine test failed with RangedTransition parameter"
			<< std::endl;
		allpass = false;
	}

	return allpass;
}
