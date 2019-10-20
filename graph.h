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


template <class IdType=int>
class GraphNode {
public:
	explicit GraphNode(const IdType & id) : m_id(id) {
	}

	IdType get_id() const {
		return m_id;
	}

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

private:
	IdType m_id;

	std::unordered_map<IdType, std::pair<const GraphNode *, unsigned>> m_linked_nodes;
};


template <class IdType=int>
class Graph {
public:
	Graph() : m_nodes() {
	}

	GraphNode<IdType> * add_node(IdType id) {
		auto it = m_nodes.find(id);
		if (it == m_nodes.end()) {
			return nullptr;
		}
		m_nodes.emplace_hint(it, id);
	}

	typedef void TraverseCallbackType(Graph & graph, GraphNode<IdType> &);
	typedef void TraverseCallbackTypeConst(const Graph & graph, const GraphNode<IdType> & node);

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

	class GraphNodeHash  {
	public:
		size_t operator () (const GraphNode<IdType> & graph_node) const {
			return std::hash<IdType>()(graph_node.get_id());
		}
	};


	std::unordered_set<GraphNode<IdType>, GraphNodeHash> m_nodes;
};


}
