#pragma once

// required by assert
#include <cassert>

// required by std::unique_ptr
#include <memory>

// required by Exception
#include "exception.h"


namespace crowbox {


// TODO: replace asserts with proper error handling


class ErrorStorageFull : public Exception {
public:
	ErrorStorageFull() : Exception() {
	}
};


class CircullarBuffer {
public:

	CircullarBuffer() :
		m_array_ptr(),
		m_array_end_ptr(nullptr),
		m_right_bndry_ptr(nullptr),
		m_push_ptr(nullptr),
		m_pop_ptr(nullptr),
		m_reserved_size(0),
		m_contiguity(true) {
	}

	CircullarBuffer(const CircullarBuffer &) = delete;

	CircullarBuffer(CircullarBuffer && copy) :
		m_array_ptr(std::move(copy.m_array_ptr)),
		m_array_end_ptr(copy.m_array_end_ptr),
		m_right_bndry_ptr(copy.m_right_bndry_ptr),
		m_push_ptr(copy.m_push_ptr),
		m_pop_ptr(copy.m_pop_ptr),
		m_reserved_size(copy.m_reserved_size),
		m_contiguity(copy.m_contiguity) {
		copy.m_array_end_ptr = nullptr;
		copy.m_right_bndry_ptr = nullptr;
		copy.m_push_ptr = nullptr;
		copy.m_pop_ptr = nullptr;
		copy.m_reserved_size = 0;
		copy.m_contiguity = false;
	}

	//CircullarBuffer & operator = (CircullarBuffer && copy) = default;

	/// Make sure that we have at least specified amount of space at
	/// get_push_addr() address
	void set_guaruanteed_push_size(size_t value) {
		m_reserved_size = value;
	}

	/// check if the first_addr address is allocated after the second_addr
	/// allocation. It is assumed that both addresses are allocated from these
	/// CircullarBuffer and are not logically released
	bool compare_allocations(const void * first_addr,
							const void * second_addr) const {
		if (m_push_ptr > m_pop_ptr) {
			if (first_addr >= m_pop_ptr && first_addr < m_push_ptr &&
				second_addr >= m_pop_ptr && second_addr < m_push_ptr) {
				return (first_addr > second_addr);
			}
			else {
				throw EXCEPTION("");
			}
		}
		else {
			if (first_addr >= m_pop_ptr) {
				if (second_addr >= m_pop_ptr) {
					return (first_addr > second_addr);
				}
				if (second_addr < m_push_ptr) {
					return false;
				}
				throw EXCEPTION("");
			}
			if (first_addr < m_push_ptr) {
				if (second_addr >= m_pop_ptr) {
					return true;
				}
				if (second_addr < m_push_ptr) {
					return (first_addr > second_addr);
				}
				throw EXCEPTION("");
			}
			throw EXCEPTION("");
		}
	}

	/// check if block were written contigous before last push method call
	bool get_contiguity_flag() const {
		return m_contiguity;
	}

	void reset() {
		m_right_bndry_ptr = m_array_end_ptr;
		m_push_ptr = m_array_ptr.get();
		m_pop_ptr = m_array_ptr.get();
	}

	void init(size_t size) {
		m_array_ptr.reset(new char[size]);
		m_array_end_ptr = m_array_ptr.get() + size;
		this->reset();
	}

	void * get_push_addr() const {
		return m_push_ptr;
	}

	void * get_pop_addr() const {
		return m_pop_ptr;
	}

	size_t get_remaining_size() const {
		if (m_push_ptr >= m_pop_ptr) {
			return m_array_end_ptr - m_push_ptr;
		}
		else {
			return m_pop_ptr - m_push_ptr;
		}
	}

	void record_write(size_t size) {
		this->push(size);
	}

	size_t get_free_space_left() const {
		if (m_push_ptr >= m_pop_ptr) {
			return (m_array_end_ptr - m_push_ptr +
					m_pop_ptr - m_array_ptr.get());
		}
		else {
			return m_pop_ptr - m_push_ptr;
		}
	}

	void push(size_t size) {
		if (m_push_ptr >= m_pop_ptr) {
			this->push_after_pop_ptr(size);
		}
		else {
			this->push_before_pop_ptr(size);
		}
	}

	void pop(size_t size) {
		this->pop_till(m_pop_ptr + size);
	}

	/// start_ptr is always == m_pop_ptr
	void pop_till(const void * end_ptr) {
		if (m_push_ptr > m_pop_ptr) {
			if (end_ptr < m_push_ptr) {
				// good, 99% case
			}
			else {
				if (end_ptr == m_push_ptr) {
					// report empty
				}
				if (end_ptr <= m_pop_ptr) {
					assert(0);
					// impossible if called from pop(size_t)
				}
				if (end_ptr > m_push_ptr) {
					// overflow, freeing more than we should
					assert(0);
					// TODO: replace assert with reliable error handling
				}
			}
		}
		else {
			if (end_ptr > m_pop_ptr) {
				if (end_ptr < m_right_bndry_ptr) {
					// good: 99% of expected hit
				}
				else {
					if (end_ptr == m_right_bndry_ptr) {
						// if we reached the right boundary we must continue
						// to pop from the beginning, because there is no data
						// that is written after right boundary
						// TODO: very rare case, we should properly prioritize
						// this case
						m_pop_ptr = m_array_ptr.get();
						return;
					}
					else {
						// if (end_ptr > m_right_bndry_ptr)
						// overflow, TODO: properly handle this error
						assert(0);
					}
				}
			}
			else {
				// TODO: propely study this case
				if (end_ptr > m_push_ptr) {
					assert(0);
				}
				else if (end_ptr < m_array_ptr.get()) {
					assert(0);
				}
				m_right_bndry_ptr = m_array_end_ptr;
			}
		}
		m_pop_ptr = static_cast<char *>(const_cast<void *>(end_ptr));
	}

private:

	void raise_event_storage_is_full() {
		// TODO: storage is full, we need to use another storage or free
		// space before we continue
		throw ErrorStorageFull();
	}

	void push_after_pop_ptr(size_t size) {
		m_push_ptr += size;
		if (m_push_ptr > m_array_end_ptr) {
			assert(0);
		}
		if (m_push_ptr + m_reserved_size > m_array_end_ptr) {
			if (m_array_ptr.get() + m_reserved_size > m_pop_ptr) {
				assert(0);
				raise_event_storage_is_full();
			}
			else {
				m_right_bndry_ptr = m_push_ptr;
				m_push_ptr = m_array_ptr.get();
				m_contiguity = false;
				return;
			}
		}
		m_contiguity = true;
	}

	void push_before_pop_ptr(size_t size) {
		m_push_ptr += size;
		if (m_push_ptr > m_pop_ptr) {
			assert(0);
		}
		if (m_push_ptr + m_reserved_size > m_pop_ptr) {
			assert(0);
			raise_event_storage_is_full();
		}
		m_contiguity = true;
	}

	std::unique_ptr<char> m_array_ptr;

	char * m_array_end_ptr;
	char * m_right_bndry_ptr;
	char * m_push_ptr;
	char * m_pop_ptr;
	size_t m_reserved_size;

	bool m_contiguity;
};

}
