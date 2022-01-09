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

		void addGraph(graph_sptr graphToAdd);
		void removeGraph(graph_sptr graphToRemove);
		void clearGraph(graph_sptr graphToSwapWith);

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