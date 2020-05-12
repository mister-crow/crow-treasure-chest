#pragma once

#include <vector>


namespace crowbox {


class RingBufferArray {
public:
	RingBufferArray();

	RingBuffer push(size_t size) {
		try {
			head_ptr->push(size);
		} catch (const ErrorStorageFull & err) {
			//m_buffer_array.emplace_back(RingBuffer());
			//m_buffer_array.push_back(b);
		}
		RingBuffer b;
		m_buffer_array.emplace_back(std::move(b));
		return b;
	}
	void pop(size_t size) {
		tail_ptr->pop(size);
	}

private:
	RingBuffer * head_ptr;
	RingBuffer * tail_ptr;
	std::vector<RingBuffer> m_buffer_array;
};

}
