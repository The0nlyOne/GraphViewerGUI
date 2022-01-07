#include "Node.hpp"

namespace Model
{

	void Node::connect(const node_sptr node, const int weight) {
		for (auto&& pairNodeWeight : vertices_) { // check if vertex is already in the Node
			if (pairNodeWeight.first == node && pairNodeWeight.second == weight) {
				return;
			}
		}
		vertices_.push_back({ node, weight });
	}

	void Node::disconnect(node_sptr node, int weight) {
		for (int i = 0; i < vertices_.size(); i++) {
			if (vertices_[i].first == node && vertices_[i].second == weight) {
				vertices_.erase(vertices_.begin() + i);
			}
		}
	}

	void Node::updateDistFoMin() {
		for (auto&& distance : minDistances_) {
			if (distance == INT_MAX || distance == INT_MIN) { continue; }
			if (distance < distForMin_) {
				distForMin_ = distance;
			}
		}
	}

	void Node::updateDistForMax() {
		for (auto&& distance : maxDistances_) {
			if (distance == INT_MAX || distance == INT_MIN) { continue; }
			if (distance > distForMax_) {
				distForMax_ = distance;
			}
		}
	}

	void Node::addDistForMin(int distance) {
		if (getLastDistForMin() == INT_MAX) {
			minDistances_[minDistances_.size() - 1] = distance;
		}
		else {
			minDistances_.push_back(distance);
		}
	}

	void Node::addDistForMax(int distance) {
		if (getLastDistForMax() == INT_MIN) {
			maxDistances_[maxDistances_.size() - 1] = distance;
		}
		else {
			maxDistances_.push_back(distance);
		}
	}

}