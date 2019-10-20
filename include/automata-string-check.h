#pragma once

// required by std::string
#include <string>

// required by Automata
#include "automata.h"


namespace crowbox {


class AutomataStringCheck {
public:
	AutomataStringCheck() : m_automata_ptr(nullptr) {
	}

	virtual ~AutomataStringCheck() = default;

    bool check(const std::string & str) const {
		using State=crowbox::Automata<char>::State;

        std::string timmed_str = trim(str);
        
		const State * current_state_ptr = m_automata_ptr->get_start_state();
        for (char symbol : timmed_str) {
			current_state_ptr = m_automata_ptr->step(current_state_ptr, symbol);
			if (!current_state_ptr) {
				return false;
			}
        }
		if (current_state_ptr->get_type() == State::final) {
			return true;
		}
        return false;
    }

	void set_automata(Automata<char> & automata) {
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
        
    Automata<char> * m_automata_ptr;
};



}
