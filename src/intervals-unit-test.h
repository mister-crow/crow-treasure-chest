#pragma once

#include <iostream>
#include <vector>
#include <exception>

#include "intervals.h"
#include "exception.h"


using crowbox::IntervalsNonIntersecting;
using crowbox::IntervalsIntersecting;


class IntervalsTester {
public:
	typedef int Number;
	typedef std::pair<Number, Number> Interval;

	void add_interval(Number start, Number end) {
		m_intervals.add_interval(start, end);
	}

	bool compare(const std::vector<Interval> & intervals_golden) {
		auto golden_interval_it = intervals_golden.begin();
		if (intervals_golden.size() != m_intervals.size()) {
			return false;
		}
		for (const Interval & interval : m_intervals) {
			if (interval != *golden_interval_it) {
				return false;
			}
			++golden_interval_it;
		}
		return true;
	}

private:

	IntervalsNonIntersecting m_intervals;
};


void intervals_unit_test_non_inter() {

	IntervalsTester tester;

	tester.add_interval(10, 20);
	if (!tester.compare({{10,20}})) {
		throw crowbox::EXCEPTION("");
	}

	tester.add_interval(21, 30);
	if (!tester.compare({{10,30}})) {
		throw crowbox::EXCEPTION("");
	}

	tester.add_interval(50, 50);
	if (!tester.compare({{10,30},{50,50}})) {
		throw crowbox::EXCEPTION("");
	}

	tester.add_interval(40, 41);
	if (!tester.compare({{10,30},{40,41},{50,50}})) {
		throw crowbox::EXCEPTION("");
	}

	tester.add_interval(20, 41);
	if (!tester.compare({{10,41},{50,50}})) {
		throw crowbox::EXCEPTION("");
	}
}

void intervals_unit_test_inter() {
	IntervalsIntersecting intervals;
	intervals.add_interval(1, 50);
	intervals.add_interval(1,2);
	intervals.add_interval(10,20);
	intervals.add_interval(20,25);
	intervals.add_interval(15,21);
	intervals.add_interval(21,21);
	intervals.add_interval(21,23);
	intervals.add_interval(13,14);
	typedef int Number;
	typedef std::pair<Number, Number> Interval;

	std::multiset<Interval> golden_intersection;
	golden_intersection.insert(Interval(1,50));
	golden_intersection.insert(Interval(10,20));
	golden_intersection.insert(Interval(15,21));
	golden_intersection.insert(Interval(20,25));

	auto intersections = intervals.find_intersections(Interval(20,20));

	if (intersections.size() != golden_intersection.size()) {
		throw crowbox::EXCEPTION("");
	}
	for (const auto & interval : intersections) {
		std::cout << interval.first << " " << interval.second << std::endl;
		if (golden_intersection.find(interval) == golden_intersection.end()) {
			throw crowbox::EXCEPTION("");
		}
	}
}

bool intervals_unit_test() {
	try {
		intervals_unit_test_non_inter();
		intervals_unit_test_inter();
	} catch  (const std::exception & err) {
		std::cerr << err.what() << std::endl;
		return false;
	}
	return true;
}
