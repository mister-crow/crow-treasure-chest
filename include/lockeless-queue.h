#pragma once


namespace crowbox {
namespace lockeless_queue {


class DataBlock {
public:
};


class Producer {
public:
	void push_data(void * pointer, size_t size);
};


class Consumer {
	void pull_data();
};


// Single Producer and Single Consumer
class LockelessQueue {
public:
};

}
}
