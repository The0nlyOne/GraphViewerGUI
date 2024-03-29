#include "Graph.hpp"

namespace Model
{

	// Useful for minHeap and maxHeap
	bool operator< (const std::shared_ptr<Vertex>& first, const std::shared_ptr<Vertex> second) noexcept {
		return (first->getWeight() < second->getWeight()) ? true : false;
	}

	void Graph::addNodeAndChildren(node_sptr node) {
		bool canAddNode = true;
		// even if node is a set, we must throw same name error because two different pointers can have nodes with same name
		for (auto&& existingNode : nodes_) {
			if (existingNode->getName() == node->getName()) {
				canAddNode = false;
			}
		}
		if (canAddNode) {
			nodes_.insert(node);
			emit nodeAddedSignal(this, node);
			for (auto&& pairNodeWeight : node->getVertices()) {
				addAllNodesAndVerticess(node, pairNodeWeight);
			}
		}
	}

	void Graph::addAllNodesAndVerticess(node_sptr parentNode, std::pair<node_sptr, int> pairNodeWeight) {
		if (findVertex(parentNode, pairNodeWeight.first, pairNodeWeight.second)) { return; } // stop if vertex already exist

		nodes_.insert(pairNodeWeight.first);
		emit nodeAddedSignal(this, pairNodeWeight.first);
		if (pairNodeWeight.first->getVertices().size() == 0) { // can use isLeaf
			leaves_.insert(pairNodeWeight.first);
		}
		addVertices(parentNode, pairNodeWeight.first, pairNodeWeight.second);
		for (auto&& nextPairNodeWeight : pairNodeWeight.first->getVertices()) {
			addAllNodesAndVerticess(pairNodeWeight.first, nextPairNodeWeight);
		}
	}

	void Graph::addVertices(node_sptr parentNode, node_sptr childNode, int weight) {
		if (findVertex(parentNode, childNode, weight)) { return; } //  stop if vertex already exist

		vertex_sptr vertex = std::make_shared<Vertex>(childNode, parentNode, weight);
		vertices_.push_back(vertex);
		verticesNeighbours_[parentNode].push_back(vertex);
		parentNodes_[childNode].push_back(parentNode);
		emit vertexAddedSignal(this, vertex);

	}

	node_sptr Graph::returnSetItem(std::unordered_set<node_sptr> unorderedSet, node_sptr firstNode) {
		std::unordered_set<node_sptr>::const_iterator nodeSptrIterator = unorderedSet.find(firstNode);
		return (nodeSptrIterator == unorderedSet.end()) ? nullptr : *nodeSptrIterator;
	}

	void Graph::connectNodes(node_sptr firstNode, node_sptr secondNode, int weight) {
		if (nodes_.empty() || !root_) {
			root_ = firstNode;
			addNodeAndChildren(firstNode);
		}
		if (returnSetItem(nodes_, firstNode)) {
			firstNode->connect(secondNode, weight);
			leaves_.erase(firstNode);
			addVertices(firstNode, secondNode, weight);
			addNodeAndChildren(secondNode);
		}
	}

	void Graph::biConnectNodes(node_sptr firstNode, node_sptr secondNode, int weight) {
		connectNodes(firstNode, secondNode, weight);
		connectNodes(secondNode, firstNode, weight);
	}

	void Graph::disconnectVertex(vertex_sptr vertex) {
		if (vertex) {
			vertex->getPreviousNode()->disconnect(vertex->getNode(), vertex->getWeight());
			for (int i = 0; i < vertices_.size(); i++) {
				if (vertices_[i] == vertex) {
					vertices_.erase(vertices_.begin() + i);
					break;
				}
			}
			for (int i = 0; i < verticesNeighbours_[vertex->getPreviousNode()].size(); i++) {
				if (verticesNeighbours_[vertex->getPreviousNode()][i] == vertex) {
					verticesNeighbours_[vertex->getPreviousNode()].erase(verticesNeighbours_[vertex->getPreviousNode()].begin() + i); // erreur car je change la taille du tableau pendant la suppression, fix� il me semble
					break;
				}
			}
			updateMinDist(root_);  // could use bool isMin to know which function to call between updateMaxDist and updateMinDist
			updateMaxDist(root_);
			emit vertexDeletedSignal(this, vertex);
		}
	}

	void Graph::disconnectNodes(node_sptr firstNode, node_sptr secondNode, int weight) {
		disconnectVertex(findVertex(firstNode, secondNode, weight));
	}

	vertex_sptr Graph::findVertex(node_sptr firstNode, node_sptr secondNode, int weight) {
		for (auto&& vertex : vertices_) {
			if (vertex->getPreviousNode() == firstNode && vertex->getNode() == secondNode && vertex->getWeight() == weight) {
				return vertex;
			}
		}
		return nullptr;
	}

	std::vector<vertex_sptr> Graph::findVertices(node_sptr firstNode, node_sptr secondNode) {
		std::vector<vertex_sptr> parentVertices;
		for (auto&& vertex : vertices_) {
			if (vertex->getPreviousNode() == firstNode && vertex->getNode() == secondNode) {
				parentVertices.push_back(vertex);
			}
		}
		return parentVertices;
	}

	void Graph::deleteNode(node_sptr nodeToDel) {
		if (nodeToDel) {
			for (auto&& node : nodes_) { // to make sure that we change the root if the current one is deleted
				if (node != nodeToDel) {
					root_ = node;
					break;
				}
			}
			nodes_.erase(nodeToDel); // does not throw exception if nodeToDel == nullptr
			
			if (nodeToDel->isLeaf()) {
				leaves_.erase(nodeToDel);
			}
			else {
				std::vector<vertex_sptr> verticeNeighbourCopy = verticesNeighbours_[nodeToDel]; // a copy so when the size of the orginal container verticeNeighbours_
				for (auto&& vertex : verticeNeighbourCopy) {									// is not modified while we iterate
					disconnectVertex(vertex);
				}
			}
			for (auto&& node : parentNodes_[nodeToDel]) {
				for (auto&& vertex : findVertices(node, nodeToDel)) {
					disconnectVertex(vertex);
				}
			}
			emit nodeDeletedSignal(this, nodeToDel);
		}
	}

	void Graph::updateMinDist(node_sptr root) {
		root_ = root;
		resetKnownVertex();
		resetNodesCount();
		resetMinValues();

		root->addDistForMin(0);
		for (auto&& vertex : verticesNeighbours_[root_]) {
			updateMinDist(vertex);
		}
		root->incrementCount();
		emit minDistUpdatedSignal(this);
	}

	void Graph::updateMaxDist(node_sptr root) {
		root_ = root;
		resetKnownVertex();
		resetNodesCount();
		resetMaxValues();

		root->addDistForMax(0);
		for (auto&& vertex : verticesNeighbours_[root_]) {
			updateMaxDist(vertex);
		}
		root->incrementCount();
		emit maxDistUpdatedSignal(this);
	}

	void Graph::updateMinDist(vertex_sptr vertex) {
		vertex->getPreviousNode()->incrementVisited();

		if (vertex->getPreviousNode()->getCount() != 0) { // check if it is the second time we are going through this node to check dist
			resetKnownChildrenNodes(vertex->getPreviousNode());
		}

		if (!vertex->getKnown()) { // updateDist here
			int tempDist = vertex->getWeight() + vertex->getPreviousNode()->getLastDistForMin();
			if (tempDist < vertex->getNode()->getLastDistForMin()) {
				vertex->getNode()->addDistForMin(tempDist);
				vertex->getNode()->setPrevNodeMin(vertex->getPreviousNode()); // use map?
			}
			vertex->setKnown(true);
		}

		for (auto&& nextVertex : verticesNeighbours_[vertex->getNode()]) { // what happen if it does not exist?
			if (nextVertex->getPreviousNode() == root_) {	// to avoid convexe/cyclique graph to go in infinite loop
				return;
			}
			if (nextVertex->getNode()->getVisited() != 0) {	// to avoid convexe/cyclique graph to go in infinite loop
				continue;
			}
			if (areBiConnectedVertices(vertex, nextVertex)) {
				vertex_sptr otherConnection = findVertex(vertex->getNode(), vertex->getPreviousNode(), vertex->getWeight());
				if (nextVertex == otherConnection) {		// to avoid convexe/cyclique graph to go in infinite loop
					continue;
				}
			}
			else {
				updateMinDist(nextVertex);
			}
		}
		for (auto&& nextVertex : verticesNeighbours_[vertex->getNode()]) {
			nextVertex->getNode()->resetVisited();
			if (nextVertex->getKnown()) {
				nextVertex->getNode()->addInfForMinVector();
			}
		}
		vertex->getNode()->incrementCount(); // increment when we finished exploring all its neighbours
	}

	void Graph::updateMaxDist(vertex_sptr vertex) {
		vertex->getPreviousNode()->incrementVisited();

		if (vertex->getPreviousNode()->getCount() != 0) {
			resetKnownChildrenNodes(vertex->getPreviousNode());
		}

		if (!vertex->getKnown()) { // updateDist here
			int tempDist = vertex->getWeight() + vertex->getPreviousNode()->getLastDistForMax();
			if (tempDist > vertex->getNode()->getLastDistForMax()) {
				vertex->getNode()->addDistForMax(tempDist);
				vertex->getNode()->setPrevNodeMax(vertex->getPreviousNode());
			}
			vertex->setKnown(true);
		}

		for (auto&& nextVertex : verticesNeighbours_[vertex->getNode()]) { // what happen if it does not exist?
			if (nextVertex->getPreviousNode() == root_) {	// to avoid convexe/cyclique graph to go in infinite loop
				return;
			}
			if (nextVertex->getNode()->getVisited() != 0) {	// to avoid convexe/cyclique graph to go in infinite loop
				continue;
			}
			if (areBiConnectedVertices(vertex, nextVertex)) {
				vertex_sptr otherConnection = findVertex(vertex->getNode(), vertex->getPreviousNode(), vertex->getWeight());
				if (nextVertex == otherConnection) {		// to avoid convexe/cyclique graph to go in infinite loop
					continue;
				}
			}
			else {
				updateMaxDist(nextVertex);
			}
		}

		for (auto&& nextVertex : verticesNeighbours_[vertex->getNode()]) {
			nextVertex->getNode()->resetVisited();
			if (nextVertex->getKnown()) {
				nextVertex->getNode()->addInfForMaxVector();
			}
		}

		vertex->getNode()->incrementCount(); // increment when we finished exploring all its neighbours
	}

	bool Graph::areBiConnectedVertices(vertex_sptr firstVertex, vertex_sptr secondVertex) {
		if (firstVertex->getNode() == secondVertex->getPreviousNode()
			&& firstVertex->getPreviousNode() == secondVertex->getNode()
			&& firstVertex->getWeight() == secondVertex->getWeight()) {
			return true;
		}
		else {
			return false;
		}
	}

	void Graph::resetKnownChildrenNodes(node_sptr root) {
		for (auto&& vertex : verticesNeighbours_[root]) {
			if (vertex->getKnown()) {
				vertex->setKnown(false);
				resetKnownChildrenNodes(vertex->getNode());
			}
		}
	}

	void Graph::resetKnownVertex() {
		for (auto&& vertex : vertices_) {
			vertex->setKnown(false);
		}
	}

	void Graph::resetNodesCount() {
		for (auto&& node : nodes_) {
			node->resetCount();
		}
	}

	void Graph::resetVisitedNodes() {
		for (auto&& node : nodes_) {
			node->resetVisited();
		}
	}

	void Graph::resetMinDistVector() {
		for (auto&& node : nodes_) {
			node->resetDistForMinVector();
		}
	}

	void Graph::resetMaxDistVector() {
		for (auto&& node : nodes_) {
			node->resetDistForMaxVector();
		}
	}

	void Graph::resetMinValues() {
		resetVisitedNodes();
		resetDistForMin();
		resetPrevForMin();
		resetMinDistVector();
	}

	void Graph::resetMaxValues() {
		resetVisitedNodes();
		resetDistForMax();
		resetPrevForMax();
		resetMaxDistVector();
	}

	void Graph::resetDistForMin() {
		for (auto&& node : nodes_) {
			node->setDistForMin(INT_MAX);
		}
	}

	void Graph::resetPrevForMin() {
		for (auto&& node : nodes_) {
			node->setPrevNodeMin(nullptr);
		}
	}

	void Graph::resetDistForMax() {
		for (auto&& node : nodes_) {
			node->setDistForMax(INT_MIN);
		}
	}

	void Graph::resetPrevForMax() {
		for (auto&& node : nodes_) {
			node->setPrevNodeMax(nullptr);
		}
	}

	void Graph::maxHeap(std::vector<vertex_sptr>* vector) {
		std::make_heap(vector->begin(), vector->end());
		std::sort_heap(vector->begin(), vector->end());
	}

	void Graph::minHeap(std::vector<vertex_sptr>* vector) {
		maxHeap(vector);
		std::reverse(vector->begin(), vector->end());
	}

	vertex_sptr Graph::getLastVertex(std::vector<vertex_sptr>* vertexVector) { // useful to test if the heapify have the lastvertex in correct order
		return !(vertexVector->empty()) ? vertexVector->back() : nullptr;
	}

}