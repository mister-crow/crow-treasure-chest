#pragma once

#include <set>
#include <map>


namespace crowbox {

typedef int Number;


template <class ContainerType, class ChildType>
class IntervalsBase {
public:
	typedef std::pair<Number, Number> Interval;
	typedef typename ContainerType::iterator iterator;
	typedef typename ContainerType::const_iterator const_iterator;

	iterator begin() {
		return m_intervals.begin();
	}
	const_iterator cbegin() const {
		return m_intervals.cbegin();
	}
	iterator end() {
		return m_intervals.end();
	}
	const_iterator cend() const {
		return m_intervals.cend();
	}
	size_t size() const {
		return m_intervals.size();
	}
	void add_interval(const Number & start, const Number & end) {
		ChildType::add_interval(start, end);
	}

	bool has_intersection(const Number & start, const Number & end) const {
		Interval interval(start, end);
		auto upper_bound_it = m_intervals.upper_bound(interval.first);
		for (auto it=m_intervals.begin(); it!=upper_bound_it; ++it) {
			const Interval & iterable_interval = *it;
			if (intersecting(interval, iterable_interval)) {
				return true;
			}
		}
		return false;
	}

	bool has_intersection(Number point) const {
		return has_intersection(point, point);
	}

	std::vector<Interval> find_intersections(const Interval & interval) const {
		std::vector<Interval> intervals;
		auto upper_bound_it = m_intervals.upper_bound(interval.first);
		for (auto it=m_intervals.begin(); it!=upper_bound_it; ++it) {
			const Interval & iterable_interval = *it;
			if (intersecting(interval, iterable_interval)) {
				intervals.push_back(iterable_interval);
			}
		}
		return intervals;
	}
	std::vector<Interval> find_intersections(Number point) const {
		Interval interval(point, point);
		return find_intersections(interval);
	}

protected:
	ContainerType m_intervals;
	void add_interval_interval(const Number & start, const Number & end) {
		m_intervals.insert(Interval(start, end));
	}

private:
	static bool intersecting(const Interval & left, const Interval & right) {
		if (left.second < right.first || left.first > right.second) {
			return false;
		}
		return true;
	}
};


class IntervalsIntersecting : public
	IntervalsBase<std::multimap<Number, Number>, IntervalsIntersecting> {
public:
	void add_interval(const Number & start, const Number & end) {
		add_interval_interval(start, end);
	}
};


class IntervalsNonIntersecting : public
	IntervalsBase<std::map<Number, Number>, IntervalsNonIntersecting> {
public:
	void add_interval(const Number & start, const Number & end) {
		Interval new_interval(start, end);
		if (m_intervals.empty()) {
			m_intervals.insert(new_interval);
			return;
		}
		auto it = m_intervals.upper_bound(end);
		if (it == m_intervals.begin()) {
			m_intervals.insert(new_interval);
			return;
		}
		if (it == m_intervals.end()) {
			auto r_it = m_intervals.rbegin();
			++r_it;
			it = r_it.base();
		}
		else {
			--it;
		}
		IntervalCompareResult result = compare_intervals(*it, new_interval);
		switch (result) {
		case IntervalCompareResult::second_follows_1st_w_gap:
			++it;
			m_intervals.insert(it, new_interval);
			break;
		case IntervalCompareResult::second_follows_1st_no_gap:
			it->second = new_interval.second;
			break;
		case IntervalCompareResult::second_contains_1st: {
			handle_multiple_inclusion_case(new_interval, ++it);
			break;
		 }
		case IntervalCompareResult::first_contains_2nd:
			// do nothing, no need to insert new interval
			break;
		case IntervalCompareResult::first_follows_2nd_w_gap:
		case IntervalCompareResult::first_follows_2nd_no_gap:
		default:
			// impossible
			assert(0);
		}
	}

private:

	/// new_interval is the new interval that should be integrated into
	/// existing interval collection that completely covers at least
	/// one stored interval
	/// upper_interval_it is the iterator to the first interval that has start
	/// greater than the end of new_interval 
	void handle_multiple_inclusion_case(
			const Interval & new_interval,
			IntervalsNonIntersecting::iterator upper_interval_it)
	{
		// find the first interval with the beginning greater or equal to the
		// new interval beginning
		auto it = m_intervals.lower_bound(new_interval.first);

		// if the beginning of the new interval is intersecting within the body
		// of another interval
		bool intersecting = false;

		if (it != m_intervals.begin()) {
			--it;
			// check for intersection
			intersecting = (it->second + 1 >= new_interval.first);
			if (intersecting) {
				// if intersecting use existing interval as a base for
				// extension
				it->second = new_interval.second;
			}
			++it;
		}
		// erase all inclusive intervals
		m_intervals.erase(it, upper_interval_it);
		if (!intersecting) {
			// if there is no intersection then new interval should be
			// inserted into the map with new key
			m_intervals.insert(upper_interval_it, new_interval);
		}
	}

	enum class IntervalCompareResult {
		second_follows_1st_w_gap,
		second_follows_1st_no_gap,
		first_follows_2nd_w_gap,
		first_follows_2nd_no_gap,
		first_contains_2nd,
		second_contains_1st
	};

	static IntervalCompareResult compare_intervals(
		const Interval & first,
		const Interval & second)
	{
		if (first.first <= second.first)
			if (first.second >= second.second)
				return IntervalCompareResult::first_contains_2nd;
			else
				if (first.second < second.first - 1)
					return IntervalCompareResult::second_follows_1st_w_gap;
				else
					return IntervalCompareResult::second_follows_1st_no_gap;
		else {
		//	if (first.first > second.second + 1)
		//		return IntervalCompareResult::first_follows_2nd_w_gap;
		//	else
		//		if (first.second > second.second)
		//			return IntervalCompareResult::first_follows_2nd_no_gap
		//		else
					return IntervalCompareResult::second_contains_1st;
		}
	}
};

}
