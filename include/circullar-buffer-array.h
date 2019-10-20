#pragma once

#include <vector>


namespace crowbox {


class CircularBufferArray {
public:
	CircularBufferArray();

	CircullarBuffer push(size_t size) {
		try {
			head_ptr->push(size);
		} catch (const ErrorStorageFull & err) {
			//m_buffer_array.emplace_back(CircullarBuffer());
			//m_buffer_array.push_back(b);
		}
		CircullarBuffer b;
		m_buffer_array.emplace_back(std::move(b));
		return b;
	}
	void pop(size_t size) {
		tail_ptr->pop(size);
	}

private:
	CircullarBuffer * head_ptr;
	CircullarBuffer * tail_ptr;
	std::vector<CircullarBuffer> m_buffer_array;
};

}
