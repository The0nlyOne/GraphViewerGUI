#include "GraphViewer.hpp"

namespace Model
{

	void GraphViewer::addGraph(graph_sptr graphToAdd) {
		for (int i = 0; i < graphsVector_.size(); i++) {
			if (graphsVector_[i]->getName() == graphToAdd->getName()) {
				graphsVector_.erase(graphsVector_.begin() + i);
				break;
			}
		}
		graphsVector_.push_back(graphToAdd);
		emit graphAddedSignal(graphToAdd);
	}

	void GraphViewer::removeGraph(graph_sptr graphToRemove) {
		for (int i = 0; i < graphsVector_.size(); i++) {
			if (graphsVector_[i] == graphToRemove) {
				graphsVector_.erase(graphsVector_.begin() + i);
				break;
			}
		}
		emit graphRemovedSignal(graphToRemove);
	}

	void GraphViewer::clearGraph(graph_sptr graphToSwapWith) {
		for (int i = 0; i < graphsVector_.size(); i++) {
			if (graphsVector_[i]->getName() == graphToSwapWith->getName()) {
				graphsVector_.erase(graphsVector_.begin() + i);
				break;
			}
		}
		graphsVector_.push_back(graphToSwapWith);
		currentGraph_ = graphToSwapWith;
		emit graphCleared(graphToSwapWith);
	}

}