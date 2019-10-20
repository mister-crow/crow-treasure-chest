#pragma once

#include <map>
#include <set>
#include <vector>
#include <forward_list>


namespace crowbox {


template <class StateSymbol=char>
class Automata {
public:

	class State;

	class StateTransition {
	public:
		explicit StateTransition(const State & target_state) :
			m_symbols(),
			m_target_state_ptr(&target_state) {
		}
		
		bool has_symbol(const StateSymbol & symbol) const {
			if (m_symbols.find(symbol)==m_symbols.end()) {
				return false;
			}
			return true;
		}
		
		const State * get_target_state() const {
			return m_target_state_ptr;
		}
		
		// TODO: consider to remove this method
		void add_symbols(const std::vector<StateSymbol> & symbols) {
			for (const StateSymbol & symbol : symbols) {
				m_symbols.insert(symbol);
			}
		}

		void add_symbol(const StateSymbol & symbol) {
			m_symbols.insert(symbol);
		}
		
	private:
		std::set<StateSymbol> m_symbols;
		const State * m_target_state_ptr;
	};


	class State {
	public:
		enum StateType {
			start = 1,
			final = 2,
			intmd = 0
		};
		
		explicit State(StateType tp=intmd) : m_type(tp) {
		}
		
		const State * find_next_state(const StateSymbol & symbol) const {
			for (const StateTransition & state_trans : m_transitions) {
				if (state_trans.has_symbol(symbol)) {
					return state_trans.get_target_state();
				}
			}
			return nullptr;
		}
		
		// TODO: consider to remove this method
		void add_transition(
			const State & target_state,
			const std::vector<StateSymbol> & symbols)
		{
			m_transitions.emplace_back(target_state);
			m_transitions.back().add_symbols(symbols);
		}

		void add_transition(
			const State & target_state,
			const StateSymbol & symbol)
		{
			m_transitions.emplace_back(target_state);
			m_transitions.back().add_symbol(symbol);
		}

		StateType get_type() const {
			return  m_type;
		}

	private:
		StateType m_type;
		std::vector<StateTransition> m_transitions;
	};


public:
    Automata() :
		m_states(),
		m_start_state_ptr(nullptr) {
		m_states.emplace_front(State::start);
		m_start_state_ptr = &(m_states.front());
    }

	State * add_state_intermediate() {
		m_states.emplace_front(State::intmd);
		return &(m_states.front());
	}

	State * add_state_final() {
		m_states.emplace_front(State::final);
		return &(m_states.front());
	}

	template <class StateSymbolSequence>
    bool process_input(const StateSymbolSequence & char_sequence) const {
        
		const State * current_state_ptr =
			this->m_automata_ptr->get_start_state();
        for (const StateSymbol & symbol : char_sequence) {
			current_state_ptr =
				this->m_automata_ptr->step(current_state_ptr, symbol);
			if (!current_state_ptr) {
				return false;
			}
        }
		if (current_state_ptr->get_type() == State::final) {
			return true;
		}
        return false;
    }

    const State * step(
			const State * current_state_ptr,
			const StateSymbol & symbol) const
	{
		const State * next_state_ptr =
			current_state_ptr->find_next_state(symbol);
		return next_state_ptr;
    }

    State * get_start_state() {
		return m_start_state_ptr;
    }
    
    const State * get_start_state() const {
		return m_start_state_ptr;
    }
    
private:
	std::forward_list<State> m_states;
	State * m_start_state_ptr;
};


}
