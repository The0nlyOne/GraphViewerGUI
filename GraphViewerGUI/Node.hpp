#pragma once

#include <string>
#include <memory>
#include <vector>
#include <utility>

namespace Model
{

	class Node {

	public:
		typedef std::shared_ptr<Node> node_sptr;

		Node() = default;

		Node(std::string name) :
			name_(name)
		{
		}

		Node(std::string name, int value) :
			name_(name),
			value_(value)
		{
		}

		Node(std::string name, const std::vector<std::pair<node_sptr, int>>& edges) :
			name_(name),
			vertices_(edges)
		{
		}

		Node(std::string name, int value, const std::vector<std::pair<node_sptr, int>>& edges) :
			name_(name),
			value_(value),
			vertices_(edges)
		{
		}

		void connect(const node_sptr node, const int weight);
		void disconnect(node_sptr node, int weight);
		bool isLeaf() { return (vertices_.size() == 0) ? true : false; }

		int getDistForMin() {
			updateDistFoMin();
			return distForMin_;
		}
		void updateDistFoMin();

		int getDistForMax() {
			updateDistForMax();
			return distForMax_;
		}
		void updateDistForMax();

		void addDistForMin(int distance);
		int getLastDistForMin() { return minDistances_[minDistances_.size() - 1]; }
		std::vector<int> getDistForMinVector() { return minDistances_; }
		void resetDistForMinVector() { minDistances_ = { INT_MAX }; }
		void addInfForMinVector() { minDistances_.push_back(INT_MAX); }

		void addDistForMax(int distance);
		int getLastDistForMax() { return maxDistances_[maxDistances_.size() - 1]; }
		void resetDistForMaxVector() { maxDistances_ = { INT_MIN }; }
		std::vector<int> getDistForMaxVector() { return maxDistances_; }
		void addInfForMaxVector() { maxDistances_.push_back(INT_MIN); }

		void setDistForMax(int distance) { distForMax_ = distance; }
		void setDistForMin(int distance) { distForMin_ = distance; }

		int getCount() { return count_; }
		void incrementCount() { count_++; }
		void resetCount() { count_ = 0; }

		int getVisited() { return visited_; }
		void incrementVisited() { visited_++; }
		void resetVisited() { visited_ = 0; }

		void setPrevNodeMin(node_sptr node) { prevNodeMin_ = node; }
		node_sptr getPrevNodeMin() { return prevNodeMin_; }
		void setPrevNodeMax(node_sptr node) { prevNodeMax_ = node; }
		node_sptr getPrevNodeMax() { return prevNodeMax_; }

		std::vector<std::pair<node_sptr, int>> getVertices() { return vertices_; }
		void setVertices(std::vector<std::pair<node_sptr, int>> vertices) { vertices_ = vertices; }
		int getValue() { return value_; }
		void setValue(int value) { value_ = value; }
		std::string getName() { return name_; }
		void setName(std::string name) { name_ = name; }

	private:
		std::string name_ = "";
		int value_ = 0;
		int distForMin_ = INT_MAX;
		int distForMax_ = INT_MIN;
		int count_ = 0;
		int visited_ = 0;
		node_sptr prevNodeMin_;
		node_sptr prevNodeMax_;
		std::vector<std::pair<node_sptr, int>> vertices_;
		std::vector<int> minDistances_ = { INT_MAX };
		std::vector<int> maxDistances_ = { INT_MIN };
	};

}