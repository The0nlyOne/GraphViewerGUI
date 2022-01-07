#pragma once

#include "Graph.hpp"
#include "Exception.hpp"

#pragma warning(push, 0)
#include <QObject>
#pragma pop()

namespace Model
{

	typedef std::shared_ptr<Graph> graph_sptr;

	class GraphViewer : public QObject
	{
		Q_OBJECT
	public:
		static GraphViewer* getGraphViewer() {
			static GraphViewer onlyInstance;
			return &onlyInstance;
		}

		void changeGraph(graph_sptr graph) { 
			currentGraph_ = graph;
			emit graphChangedSignal(graph);
		}

		void addGraph(graph_sptr graphToAdd) {
			for (int i = 0; i < graphsVector_.size(); i++) {
				if (graphsVector_[i]->getName() == graphToAdd->getName()) {
					graphsVector_.erase(graphsVector_.begin() + i);
					break;
					//throw SameName("A Graph with this name already exist.\n");
				}
			}
			graphsVector_.push_back(graphToAdd);
			emit graphAddedSignal(graphToAdd);
		}

		void removeGraph(graph_sptr graphToRemove) {
			for (int i = 0; i < graphsVector_.size(); i++) {
				if (graphsVector_[i] == graphToRemove) {
					graphsVector_.erase(graphsVector_.begin() + i);
					break;
				}
			}
			emit graphRemovedSignal(graphToRemove);
		}

		void clearGraph(graph_sptr graphToSwapWith) {
			for (int i = 0; i < graphsVector_.size(); i++) {
				if (graphsVector_[i]->getName() == graphToSwapWith->getName()) {
					graphsVector_.erase(graphsVector_.begin() + i);
					// emit graphAddedSignal(graphToSwapWith);
					break;
				}
			}
			graphsVector_.push_back(graphToSwapWith);
			currentGraph_ = graphToSwapWith;
			emit graphCleared(graphToSwapWith);
		}

		graph_sptr getCurrentGraph() { return currentGraph_; }
		std::vector<graph_sptr> getGraphsVector() { return graphsVector_; }

	signals:
		void graphChangedSignal(graph_sptr currentGraph);
		void graphAddedSignal(graph_sptr graphAdded);
		void graphRemovedSignal(graph_sptr graphRemoved);
		void graphCleared(graph_sptr graphToReplaceWith);
	private:
		GraphViewer() = default;
		std::vector<graph_sptr> graphsVector_;
		graph_sptr currentGraph_ =  nullptr;
	};

}