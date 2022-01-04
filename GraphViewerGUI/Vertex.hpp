#pragma once

#include "Node.hpp"

namespace Model
{
	typedef std::shared_ptr<Node> node_sptr;

	class Vertex {
		typedef std::shared_ptr<Vertex> vertex_sptr;
	public:

		Vertex(const node_sptr node, const node_sptr previous, const int weight) :
			node_(node),
			previousNode_(previous),
			weight_(weight)
		{
		}

		bool operator< (const Vertex& other) const {
			return (weight_ < other.getWeight()) ? true : false;
		}

		void setKnown(bool value) { known_ = value; }
		bool getKnown() const { return known_; }

		node_sptr getNode() const { return node_; }
		node_sptr getPreviousNode() const { return previousNode_; }

		int getWeight() const { return weight_; }

	private:
		bool known_ = false;
		node_sptr node_;
		node_sptr previousNode_;
		int weight_;

	};
}

