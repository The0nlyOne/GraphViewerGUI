#pragma once

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <stack>

#include "Vertex.hpp"
#include "Node.hpp"
#include "Exception.hpp"

#pragma warning(push, 0)
#include <QObject>
#pragma pop()

namespace Model
{
	typedef std::shared_ptr<Node> node_sptr;
	typedef std::shared_ptr<Vertex> vertex_sptr;

	class Graph : public QObject 
	{
		Q_OBJECT
	public:
		Graph() = default;

		Graph(std::string name) :
			name_(name)
		{}

		Graph(node_sptr root) :
			root_(root)
		{
			addNodeAndChildren(root_);
		}

		Graph(std::string name, node_sptr root) :
			name_(name), root_(root)
		{
			addNodeAndChildren(root_);
		}

		void addNodeAndChildren(node_sptr node);
		void addAllNodesAndVerticess(node_sptr parentNode, std::pair<node_sptr, int> pairNodeWeight);
		void addVertices(node_sptr parentNode, node_sptr childNode, int weight);

		node_sptr returnSetItem(std::unordered_set<node_sptr> unorderedSet, node_sptr firstNode);

		void connectNodes(node_sptr firstNode, node_sptr secondNode, int weight);
		void biConnectNodes(node_sptr firstNode, node_sptr secondNode, int weight);
		void deleteNode(node_sptr nodeToDel);

		void disconnectVertex(vertex_sptr vertex);
		void disconnectNodes(node_sptr firstNode, node_sptr secondNode, int weight);

		vertex_sptr findVertex(node_sptr firstNode, node_sptr secondNode, int weight);
		std::vector<vertex_sptr> findVertices(node_sptr firstNode, node_sptr secondNode);
		vertex_sptr getLastVertex(std::vector<vertex_sptr>* vertexVector); // useful to test if the heapify have the lastvertex in correct order

		void updateMinDist(node_sptr root);
		void updateMaxDist(node_sptr root);

		void updateMinDist(vertex_sptr vertex);
		void updateMaxDist(vertex_sptr vertex);

		bool areBiConnectedVertices(vertex_sptr firstVertex, vertex_sptr secondVertex);

		void resetKnownChildrenNodes(node_sptr root);
		void resetKnownVertex();
		void resetNodesCount();
		void resetVisitedNodes();
		void resetMinDistVector();
		void resetMaxDistVector();
		void resetMinValues();
		void resetMaxValues();
		void resetDistForMin();
		void resetPrevForMin();
		void resetDistForMax();
		void resetPrevForMax();

		void maxHeap(std::vector<vertex_sptr>* vector); // can be global method? Not used now but can be useful
		void minHeap(std::vector<vertex_sptr>* vector); // can be global method? Not used now but can be useful

		node_sptr getRoot() { return root_; }
		void  setRoot(node_sptr root) { root_ = root; }

		std::unordered_set<node_sptr> getNodes() { return nodes_; }
		std::unordered_set<node_sptr> getLeaves() { return leaves_; }
		std::vector<vertex_sptr> getVertices() { return vertices_; }
		std::unordered_map<node_sptr, std::vector<vertex_sptr>> getVerticesNeighbours() { return verticesNeighbours_; }
		std::unordered_map<node_sptr, std::vector<node_sptr>> getParentNodes() { return parentNodes_; }
		std::string getName() { return name_; }
		void setName(std::string name) { name_ = name; }

	signals:
		void nodeAddedSignal(Graph* graph, node_sptr node);
		void nodeDeletedSignal(Graph* graph, node_sptr node);
		void vertexAddedSignal(Graph* graph, vertex_sptr vertex);
		void vertexDeletedSignal(Graph* graph, vertex_sptr vertex);
		void minDistUpdatedSignal(Graph* graph);
		void maxDistUpdatedSignal(Graph* graph);

	private:
		std::string name_;
		node_sptr root_; // use unique_ptr instead?
		std::unordered_set<node_sptr> nodes_; // using unordered set because most of its method are O(1) for my utilisation
		std::unordered_set<node_sptr> leaves_; // and because when the graph is convexe, other containers will have duplicate.
												// Could use a wentThrough attribute to fix that but it will be more line of code, and time complexity
		std::vector<vertex_sptr> vertices_;
		std::unordered_map<node_sptr, std::vector<vertex_sptr>> verticesNeighbours_;
		std::unordered_map<node_sptr, std::vector<node_sptr>> parentNodes_;
	};

}