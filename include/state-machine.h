#pragma once

#include <array>
#include <map>
#include <set>
#include <vector>
#include <forward_list>

#include "intervals.h"


namespace crowbox {


typedef char Symbol;


template <class Transition>
class State;


template <class Transition>
class TransitionBase {
public:
	typedef State<Transition> StateType;

	TransitionBase() : m_target_state_ptr(nullptr) {
	}

	explicit TransitionBase(const StateType & target_state)
		: m_target_state_ptr(&target_state) {
	}

	void add_symbol(const Symbol & symbol) {
		this->Transition::add_symbol(symbol);
	}

	bool has_symbol(const Symbol & symbol) const {
		return this->Transition::has_symbol(symbol);
	}

	const StateType * get_target_state() const {
		return m_target_state_ptr;
	}
	
protected:
	const StateType * m_target_state_ptr;
};


/// Optimized for O(1) access, consumes 256 bytes of array for each transition
class FastTransition : public TransitionBase<FastTransition> {
public:
	typedef State<FastTransition> StateType;

	FastTransition() = default;

	explicit FastTransition(const StateType & target_state) :
		TransitionBase<FastTransition>(target_state),
		m_symbols()
	{
		m_symbols.fill(0);
	}

	bool has_symbol(const Symbol & symbol) const {
		return m_symbols[symbol];
	}

	void add_symbol(const Symbol & symbol) {
		m_symbols[symbol] = true;
	}

private:
	std::array<bool, 255> m_symbols;
};


/// Optimized for less memory usage with log(n) access time
class CompactTransition : public TransitionBase<CompactTransition> {
public:
	typedef State<CompactTransition> StateType;

	explicit CompactTransition(const StateType & target_state) :
		TransitionBase<CompactTransition>(target_state),
		m_symbols() {
	}
	
	bool has_symbol(const Symbol & symbol) const {
		if (m_symbols.find(symbol)==m_symbols.end()) {
			return false;
		}
		return true;
	}
	
	void add_symbol(const Symbol & symbol) {
		m_symbols.insert(symbol);
	}
	
private:
	std::set<Symbol> m_symbols;
};


/// Optimized for a case when transition is associated with range of symbols
class RangedTransition : public TransitionBase<RangedTransition> {
public:
	typedef State<RangedTransition> StateType;

	explicit RangedTransition(const StateType & target_state) :
		TransitionBase<RangedTransition>(target_state),
		m_intervals() {
	}
	
	bool has_symbol(const Symbol & symbol) const {
		return m_intervals.has_intersection(symbol);
	}

	void add_range(const Symbol & start, const Symbol & end) {
		m_intervals.add_interval(start, end);
	}
	
	void add_symbol(const Symbol & symbol) {
		this->add_range(symbol, symbol);
	}
	
private:
	std::map<Symbol, Symbol> m_symbols;

	IntervalsNonIntersecting m_intervals;
};


enum class StateType {
	start = 1,
	final = 2,
	intmd = 0
};
	

template <class StateTransition>
class State {
public:
	explicit State(StateType tp=StateType::intmd) : m_type(tp) {
	}
	
	const State * find_next_state(const Symbol & symbol) const {
		for (const StateTransition & state_trans : m_transitions) {
			if (state_trans.has_symbol(symbol)) {
				return state_trans.get_target_state();
			}
		}
		return nullptr;
	}
	
	void add_transition(
		const State & target_state,
		const std::vector<Symbol> & symbols)
	{
		m_transitions.emplace_back(target_state);
		for (const Symbol & symbol : symbols) {
			m_transitions.back().add_symbol(symbol);
		}
	}

	void add_transition(
		const State & target_state,
		const Symbol & symbol)
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


template <class Transition>
class StateMachine {
public:
	typedef State<Transition> State;

public:
    StateMachine() :
		m_states(),
		m_start_state_ptr(nullptr) {
		m_states.emplace_front(StateType::start);
		m_start_state_ptr = &(m_states.front());
    }

	State * add_state_intermediate() {
		m_states.emplace_front(StateType::intmd);
		return &(m_states.front());
	}

	State * add_state_final() {
		m_states.emplace_front(StateType::final);
		return &(m_states.front());
	}

    const State * step(
		const State * current_state_ptr,
		const Symbol & symbol) const
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

	template <class SymbolContainer>
    bool process_input_symbols(const SymbolContainer & input_symbols) const {
		const State * current_state_ptr = get_start_state();
        for (const Symbol & symbol : input_symbols) {
			current_state_ptr = step(current_state_ptr, symbol);
			if (!current_state_ptr) {
				return false;
			}
        }
		if (current_state_ptr->get_type() == StateType::final) {
			return true;
		}
        return false;
    }
    
private:
	std::forward_list<State> m_states;
	State * m_start_state_ptr;
};

}
