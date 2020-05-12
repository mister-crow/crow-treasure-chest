#pragma once

// required by std::string
#include <string>

// required by StateMachine
#include "state-machine.h"


namespace crowbox {


template <class StateTransition>
class StateMachineStringCheck {
public:
	StateMachineStringCheck() : m_automata_ptr(nullptr) {
	}

	virtual ~StateMachineStringCheck() = default;

    bool check(const std::string & str) const {
        std::string trimmed_str = trim(str);
		return m_automata_ptr->process_input_symbols(trimmed_str);
    }

	void set_automata(StateMachine<StateTransition> & automata) {
		this->m_automata_ptr = &automata;
	}

private:
    static std::string trim(const std::string & s) {
        const char whitespaces[] = " \t";
        std::string trimmed = s;
        trimmed.erase(0, trimmed.find_first_not_of(whitespaces));
        trimmed.erase(trimmed.find_last_not_of(whitespaces)+1);
        return trimmed;
    }
        
    StateMachine<StateTransition> * m_automata_ptr;
};

}
