#pragma once


// required by crowbox::StateMachine
#include "state-machine.h"

// required by crowbox::StateMachineStringCheck
#include "state-machine-string-check.h"


template <class StateTransition>
class StringNumberCheck {
public:
    StringNumberCheck () :
		m_automata_ptr(build_automata()),
		m_automata_string_check() {
		m_automata_string_check.set_automata(*m_automata_ptr);
    }

	bool is_number(const std::string input) {
		return m_automata_string_check.check(input);
	}
    
private:
	typedef crowbox::StateMachine<StateTransition> StateMachine;
	std::unique_ptr<StateMachine> m_automata_ptr;
	crowbox::StateMachineStringCheck<StateTransition> m_automata_string_check;
    
    static std::unique_ptr<StateMachine> build_automata() {
        
		std::unique_ptr<StateMachine> automata_ptr =
			std::make_unique<StateMachine>();

		std::vector<char> digits =
			{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
		std::vector<char> signs = {'+', '-'};
		std::vector<char> e = {'e'};
		std::vector<char> dot = {'.'};

		typedef typename StateMachine::State State;

        State * s_start_ptr = automata_ptr->get_start_state();

        State * s_1_ptr = automata_ptr->add_state_intermediate();
		State * s_2_ptr = automata_ptr->add_state_final();
		State * s_3_ptr = automata_ptr->add_state_final();
		State * s_4_ptr = automata_ptr->add_state_final();
		State * s_5_ptr = automata_ptr->add_state_intermediate();
		State * s_6_ptr = automata_ptr->add_state_final();
        
        State * s_7_ptr = automata_ptr->add_state_intermediate();
        State * s_8_ptr = automata_ptr->add_state_intermediate();

		s_start_ptr->add_transition(*s_1_ptr, signs); 
		s_start_ptr->add_transition(*s_2_ptr, digits);
        s_start_ptr->add_transition(*s_7_ptr, dot);
		s_1_ptr->add_transition(*s_2_ptr, digits);
        s_1_ptr->add_transition(*s_7_ptr, dot);
		s_2_ptr->add_transition(*s_2_ptr, digits);
		s_2_ptr->add_transition(*s_3_ptr, dot);
		s_2_ptr->add_transition(*s_5_ptr, e);
		s_3_ptr->add_transition(*s_4_ptr, digits);
        s_3_ptr->add_transition(*s_5_ptr, e);
		s_4_ptr->add_transition(*s_5_ptr, e);
        s_4_ptr->add_transition(*s_4_ptr, digits);
		s_5_ptr->add_transition(*s_6_ptr, digits);
        s_5_ptr->add_transition(*s_8_ptr, signs);
		s_6_ptr->add_transition(*s_6_ptr, digits);
        s_7_ptr->add_transition(*s_3_ptr, digits);
        s_8_ptr->add_transition(*s_6_ptr, digits);

		return automata_ptr;
    }
};


