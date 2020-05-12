#pragma once

// required std::forward_list
#include <forward_list>

// required std::unordered_map
#include <unordered_map>

// required std::unordered_set
#include <unordered_set>

// required by std::set
#include <set>

// required by std::hash
#include <functional>


namespace crowbox {


template <class IdType>
class GraphNode {
public:
	GraphNode() = default;

	unsigned get_link(const GraphNode & node) const {
		auto it = m_linked_nodes.find(node.get_id());
		if (it == m_linked_nodes.end()) {
			return -1;
		}
		return it->second.second;
	}

	void set_link(const GraphNode & node, unsigned distance) {
		auto it = m_linked_nodes.find(node.get_id());
		if (it == m_linked_nodes.end()) {
			m_linked_nodes.emplace_hint(it, std::make_pair(&node, distance));
		}
		else {
			it->second = distance;
		}
	}

	void unlink(const GraphNode & node) {
		auto it = m_linked_nodes.find(node.get_id());
		if (it != m_linked_nodes.end()) {
			m_linked_nodes.remove(it);
		}
	}

	std::vector<const GraphNode *> get_linked_nodes() const {
		std::vector<const GraphNode *> linked_nodes;
		for (auto linked_node : m_linked_nodes) {
			const GraphNode * graph_link_node_ptr = linked_node.second.first;
			linked_nodes.push_back(graph_link_node_ptr);
		}
		return linked_nodes;
	}
	
	bool operator < (const GraphNode & other_node) const {
		return (this->m_id < other_node.m_id);
	}

	bool operator == (const GraphNode & other_node) const {
		return (this->m_id == other_node.m_id);
	}

	// TODO: too expensive?
	typedef std::unordered_map<IdType, std::pair<const GraphNode *, unsigned>>
			NodeContainerType;

	class Iterator {
	public:
		typename NodeContainerType::value_type & operator -> () {
			return *it;
		}

		typename NodeContainerType::iterator operator ++ () {
			return ++it;
		}

	private:
		typename NodeContainerType::iterator it;

	};

	static typename NodeContainerType::iterator end() {
		return NodeContainerType::end();
	}

	Iterator begin() {
		return m_linked_nodes.begin();
	};

private:
	NodeContainerType m_linked_nodes;
	IdType m_id;
};


template <class IdType=int>
class GraphNamedNodeId {
public:
	GraphNamedNodeId(int id) : m_id(id), m_name() {
	}

	void set_name(const std::string & name) {
		this->m_name = name;
	}

	int get_id() const {
		return this->m_id;
	}

	std::string get_name() const {
		return this->m_name;
	}
	const std::string & get_name_ref() const {
		return this->m_name;
	}

	bool operator == (const GraphNamedNodeId & other) const {
		return this->m_id == other.m_id;
	}

	bool operator != (const GraphNamedNodeId & other) const {
		return !(this->operator == (other));
	}

	bool operator < (const GraphNamedNodeId & other) const {
		return this->m_id < other.m_id;
	}

private:
	IdType m_id;
	std::string m_name;

	std::unordered_map<IdType, std::pair<const GraphNode<> *, unsigned>>
		m_linked_nodes;
};


template <class IdType=int>
class Graph {
public:
	Graph() : m_nodes() {
	}

	GraphNode<IdType> * add_node(IdType id) {
		auto it = m_nodes.find(id);
		if (it != m_nodes.end()) {
			return nullptr;
		}
		auto new_node_it = m_nodes.emplace_hint(
			it, std::make_pair(id, GraphNode<IdType>()));
		GraphNode<IdType> & new_node_ref = new_node_it->second;
		return &new_node_ref;
	}

	typedef void TraverseCallbackType(Graph & graph, GraphNode<IdType> &);
	typedef void TraverseCallbackTypeConst(
				const Graph & graph, const GraphNode<IdType> & node);

	void traverse_graph(TraverseCallbackType callback_function) {
		for (GraphNode<IdType> & node : m_nodes) {
			callback_function(*this, node);
		}
	}

	void traverse_graph(TraverseCallbackTypeConst callback_function) const {
		for (const GraphNode<IdType> & node : m_nodes) {
			callback_function(*this, node);
		}
	}

private:
	typedef std::unordered_map<IdType, GraphNode<IdType>>
			GraphNodeCollectionType;

	GraphNodeCollectionType m_nodes;
};

}
