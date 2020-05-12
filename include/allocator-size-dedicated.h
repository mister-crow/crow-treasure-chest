#pragma once

#include <set>
#include <map>
#include <unordered_map>


namespace crowbox {


//struct MemoryBlock {
//public:
//	MemoryBlock(void * addr, size_t size) : m_addr(addr), m_size(size) {
//	}
//
//	void * get_addr() const {
//		return m_addr;
//	}
//
//	size_t get_size() const {
//		return m_size;
//	}
//
//	bool contains(void * addr) {
//		if (addr >= m_addr && addr < m_addr + m_size) {
//			return true;
//		}
//		else {
//			return false;
//		}
//	}
//
//private:
//	void * m_addr;
//	size_t m_size;
//};
//
//
//class ArrayPool {
//public:
//	ArrayPool();
//
//
//	~ArrayPool() {
//		release_pool();
//	}
//
//	void release_pool() {
//		for (void * array_ptr : m_arrays) {
//			delete [] array_ptr;
//		}
//	}
//
//	void set_next_allocation_size(size_t size) {
//		m_next_allocation_size;
//	}
//
//	MemoryBlock allocate() {
//		if (m_prev_operation == LastOperationType::allocate) {
//			increase_next_allocation_size();
//		}
//		else {
//			m_prev_operation = LastOperationType::allocate;
//		}
//		void * new_array = new char[m_next_allocation_size];
//		// TODO: check for failure
//		if (!new_array) {
//			return nullptr;
//		}
//		m_arrays.insert(new_array);
//		return MemoryBlock(m_next_allocation_size, new_array);
//	}
//
//	void free(void * addr) {
//		m_arrays.erase(addr);
//		if (m_prev_operation == LastOperationType::release) {
//			decrease_next_allocation_size();
//		}
//		else {
//			m_prev_operation = LastOperationType::release;
//		}
//		delete [] addr;
//	}
//private:
//
//	void increase_next_allocation_size() {
//		m_next_allocation_size *= 2;
//	}
//
//	void decrease_next_allocation_size() {
//		m_next_allocation_size /= 2;
//		if (m_next_allocation_size < minimum_size) {
//			m_next_allocation_size = minimum_size;
//		}
//	}
//
//	enum LastOperationType {
//		allocate = 1,
//		release = 2
//	}
//	static constexpr size_t minimum_size = 0xfff;
//	LastOperationType m_prev_operation;
//	void m_next_allocation_size;
//	std::set<void *> m_arrays;
//};


class AllocatorFixedBlockSizeSection {
public:
	AllocatorFixedBlockSizeSection(size_t block_size, size_t number_of_blocks) : 
		m_number_of_blocks_capacity(number_of_blocks),
		m_number_of_free_blocks(number_of_blocks),
		m_block_size(block_size),
		m_top_free_block_header_ptr(nullptr)
	{
	}

	// disable copy
	AllocatorFixedBlockSizeSection(const AllocatorFixedBlockSizeSection &) = delete;
	// enable move
	AllocatorFixedBlockSizeSection(AllocatorFixedBlockSizeSection &&) = default;

	char * get_section_ptr() {
		return m_section_ptr.get();
	}

	size_t get_section_size() const {
		size_t total_block_size = m_block_size + sizeof(void *);
		return total_block_size * m_number_of_blocks_capacity;
	}

	void initialize() {
		m_number_of_free_blocks = m_number_of_blocks_capacity;

		// allocate memory
		m_section_ptr.reset(new char[get_section_size()]);

		// setup linked liist of chain of free blocks
		char * current_block_header_ptr = m_section_ptr.get();
		m_top_free_block_header_ptr = current_block_header_ptr;
		for (unsigned i=0;i<m_number_of_blocks_capacity-1;++i) {
			char * next_block_header_ptr =
				current_block_header_ptr + m_block_size + sizeof(void *);
			*reinterpret_cast<char **>(current_block_header_ptr) = next_block_header_ptr;
			current_block_header_ptr = next_block_header_ptr;
		}
		// pointer of the last block should be null
		*reinterpret_cast<char **>(current_block_header_ptr) = nullptr;
	}

	size_t get_capacity_num_of_blocks() const {
		return m_number_of_blocks_capacity;
	}

	size_t get_num_of_free_blocks() const {
		return m_number_of_free_blocks;
	}

	void * allocate() {
		if (!m_top_free_block_header_ptr) {
			return nullptr;
		}
		char * allocated_block_ptr =
			m_top_free_block_header_ptr + sizeof(void *);
		m_top_free_block_header_ptr =
			*reinterpret_cast<char **>(m_top_free_block_header_ptr);
		--m_number_of_free_blocks;
		return allocated_block_ptr;
	}

	void free(void * block_ptr) {
		char * block_header_ptr =
			reinterpret_cast<char *>(block_ptr) - sizeof(void *);
		*reinterpret_cast<char **>(block_header_ptr) = m_top_free_block_header_ptr;
		m_top_free_block_header_ptr = block_header_ptr;
		++m_number_of_free_blocks;
	}

private:
	std::unique_ptr<char []> m_section_ptr;

	size_t m_number_of_blocks_capacity;
	size_t m_number_of_free_blocks;
	size_t m_block_size;
	char * m_top_free_block_header_ptr;
};


class AllocatorFixedBlockSizeExtendable {
public:
	explicit AllocatorFixedBlockSizeExtendable(size_t size) :
		m_top_section_ptr(nullptr),
		m_block_size(size),
		m_section_pool() {
	}

	void initialize(unsigned initial_capacity_num = 100) {
		m_section_pool.clear();
		add_section(initial_capacity_num);
	}

	void * allocate() {
		void * block_ptr = m_top_section_ptr->allocate();
		if (!block_ptr) {
			size_t new_size = m_top_section_ptr->get_section_size() * 3;
			add_section(new_size);
			return m_top_section_ptr->allocate();
		}
		return block_ptr;
	}

	void free(void * block_ptr) {
		auto it = m_section_pool.lower_bound(reinterpret_cast<char *>(block_ptr));
		if (it == m_section_pool.end()) {
			assert(0);
		}
		AllocatorFixedBlockSizeSection & section = it->second;
		section.free(block_ptr);
		if (section.get_num_of_free_blocks() == section.get_capacity_num_of_blocks()) {
			if (!m_delete_candidates.empty()) {
				// m_delete_candidates.begin() != it->first;
				// TODO: remove all section from m_delete_candidates
			}
			m_delete_candidates.insert(it->first);
		}
	}

private:
	void add_section(unsigned capacity_num) {
		AllocatorFixedBlockSizeSection section(m_block_size, capacity_num);
		section.initialize();
		m_section_pool.emplace(
			std::make_pair(
				section.get_section_ptr() + section.get_section_size(),
				std::move(section))
		);
		m_top_section_ptr = &(m_section_pool.begin()->second);
	}

	AllocatorFixedBlockSizeSection * m_top_section_ptr;
	size_t m_block_size;

	std::map<char *, AllocatorFixedBlockSizeSection> m_section_pool;

	std::set<char *> m_delete_candidates;
};


class AllocatorSizeDedicated {
public:
private:
	std::unordered_map<size_t, AllocatorFixedBlockSizeExtendable> m_allocators;
};
