
class K {
public:
	int get_id() const {
		return 0;
	}


	bool operator < (const K & other_k) const {
		return (this->m_id < other_k.m_id);
	}

private:

	int m_id;
};


namespace std {
	template <>
	struct hash<K> {
		size_t operator () (const K & k) const {
			return std::hash<int>()(k.get_id());
		}
	};
}


