#pragma once

#include "GraphViewer.hpp"

namespace Model
{

	typedef std::shared_ptr<Graph> graph_sptr;

	// polymorph class because it has a virtual method
	// interface Command
	class Command
	{
	public:
		Command(const Command&) = delete; // no copy constructor for polymorph class
		Command& operator=(const Command&) = delete;  // no = constructor for polymorph class
		Command() = default;
		virtual ~Command() = default; // virtual destructor for polymorph class

		virtual void execute() = 0;
		virtual void cancel() = 0;
		virtual void redo() = 0;
	};

	class AddNode : public Command
	{
	public:
		AddNode(graph_sptr graph, node_sptr nodeToAdd) :
			currentGraph_(graph), nodeToAdd_(nodeToAdd) {}
		void execute() override { currentGraph_->addNodeAndChildren(nodeToAdd_); }
		void cancel() override { currentGraph_->deleteNode(nodeToAdd_); }
		void redo() override { currentGraph_->addNodeAndChildren(nodeToAdd_); }

	private:
		graph_sptr currentGraph_;
		node_sptr nodeToAdd_;
	};

	class removeNode : public Command
	{
	public:
		removeNode(graph_sptr graph, node_sptr nodeToRemove) :
			currentGraph_(graph), nodeToRemove_(nodeToRemove) {
			nodeBeforeDelVertices_ = nodeToRemove->getVertices();
			parentNodesBeforeDel_ = currentGraph_->getParentNodes()[nodeToRemove];
			for (auto&& parentNode : parentNodesBeforeDel_) {
				for (auto&& childNodeAndWeight : parentNode->getVertices()) {
					if (childNodeAndWeight.first == nodeToRemove) {
						nodeToRemoveParents_.push_back({ parentNode, childNodeAndWeight.second }); // do not break here cause one node can be connected multiple time to the same node
					}
				}
			}
		}
		void execute() override { currentGraph_->deleteNode(nodeToRemove_); }
		void cancel() override { 
			currentGraph_->addNodeAndChildren(nodeToRemove_);
			for (auto&& chilNodeAndWeight : nodeBeforeDelVertices_) {
				currentGraph_->connectNodes(nodeToRemove_, chilNodeAndWeight.first, chilNodeAndWeight.second); // will call add node and vertex methods that will emit signal
			}
			for (auto&& parentNodeAndWeight : nodeToRemoveParents_) {
				currentGraph_->connectNodes(parentNodeAndWeight.first, nodeToRemove_, parentNodeAndWeight.second);
			}
		}
		void redo() override { currentGraph_->deleteNode(nodeToRemove_); }

	private:
		graph_sptr currentGraph_;
		node_sptr nodeToRemove_;
		std::vector<std::pair<node_sptr, int>> nodeBeforeDelVertices_;
		std::vector<node_sptr> parentNodesBeforeDel_;
		std::vector<std::pair<node_sptr, int>> nodeToRemoveParents_;
	};

	class ConnectNodes : public Command
	{
	public:
		ConnectNodes(graph_sptr currentGraph, node_sptr firstNode, node_sptr secondNode, int weight) :
			currentGraph_(currentGraph), firstNode_(firstNode), secondNode_(secondNode), weight_(weight) {}
		void execute() override { currentGraph_->connectNodes(firstNode_, secondNode_, weight_); }
		void cancel() override { currentGraph_->disconnectNodes(firstNode_, secondNode_, weight_); }
		void redo() override { currentGraph_->connectNodes(firstNode_, secondNode_, weight_); }
	private:
		graph_sptr currentGraph_;
		node_sptr firstNode_;
		node_sptr secondNode_;
		int weight_;
	};

	class DisconnectNodes : public Command
	{
	public:
		DisconnectNodes(graph_sptr currentGraph, node_sptr firstNode, node_sptr secondNode, int weight) :
			connectNodesCmd_(currentGraph, firstNode, secondNode, weight) {}
		void execute() override { connectNodesCmd_.cancel(); }
		void cancel() override { connectNodesCmd_.execute(); }
		void redo() override { connectNodesCmd_.cancel(); }
	private:
		ConnectNodes connectNodesCmd_; // DisconnectNodes is inverse of ConnectNode
	};

	// Not used
	class BiConnectNodes : public Command
	{
	public:
		BiConnectNodes(graph_sptr currentGraph, node_sptr firstNode, node_sptr secondNode, int weight) :
			currentGraph_(currentGraph), firstNode_(firstNode), secondNode_(secondNode), weight_(weight) {}
		void execute() override { currentGraph_->biConnectNodes(firstNode_, secondNode_, weight_); }
		void cancel() override {
			currentGraph_->disconnectNodes(firstNode_, secondNode_, weight_);
			currentGraph_->disconnectNodes(secondNode_, firstNode_, weight_);
		}
		void redo() override { currentGraph_->biConnectNodes(firstNode_, secondNode_, weight_); }
	private:
		graph_sptr currentGraph_;
		node_sptr firstNode_;
		node_sptr secondNode_;
		int weight_;
	};

	// Not used
	class BiDisconnectNodes : public Command
	{
	public:
		BiDisconnectNodes(graph_sptr currentGraph, node_sptr firstNode, node_sptr secondNode, int weight) :
			biConnectNodesCmd_(currentGraph, firstNode, secondNode, weight) {}
		void execute() override { biConnectNodesCmd_.cancel(); }
		void cancel() override { biConnectNodesCmd_.execute(); }
		void redo() override { biConnectNodesCmd_.cancel(); }
	private:
		BiConnectNodes biConnectNodesCmd_; // BiDisconnectNodes is inverse of ConnectNode
	};

	// Not used
	class DisconnectVertex : public Command
	{
	public:
		DisconnectVertex(graph_sptr graph, vertex_sptr vertexToDisconnect) :
			currentGraph_(graph), vertexToDisconnect_(vertexToDisconnect) {}
		void execute() override { currentGraph_->disconnectVertex(vertexToDisconnect_); }
		void cancel() override { currentGraph_->connectNodes(vertexToDisconnect_->getPreviousNode(), vertexToDisconnect_->getNode(), vertexToDisconnect_->getWeight()); }
		void redo() override { currentGraph_->disconnectVertex(vertexToDisconnect_); }

	private:
		graph_sptr currentGraph_;
		vertex_sptr vertexToDisconnect_;
	};

	// Not used
	class ConnectVertex : public Command // more or less useless
	{
	public:
		ConnectVertex(graph_sptr graph, vertex_sptr vertexToDisconnect) :
			disconnectVertexCmd_(graph, vertexToDisconnect) {}
		void execute() override { disconnectVertexCmd_.cancel(); }
		void cancel() override { disconnectVertexCmd_.execute(); }
		void redo() override { disconnectVertexCmd_.cancel(); }

	private:
		DisconnectVertex disconnectVertexCmd_; // connectVertex is inverse of DisconnectVertex
	};

	// Not used
	class UpdateMinDist : public Command  // should it be a cmd?? since we can't cancel...
	{
	public:
		UpdateMinDist(graph_sptr graph, node_sptr root) :
			currentGraph_(graph), root_(root) {}
		void execute() override { currentGraph_->updateMinDist(root_); }
		void cancel() override {}
		void redo() override { currentGraph_->updateMinDist(root_); }
	private:
		graph_sptr currentGraph_;
		node_sptr root_;
	};

	// Not used
	class UpdateMaxDist : public Command // should it be a cmd?? since we can't cancel...
	{
	public:
		UpdateMaxDist(graph_sptr graph, node_sptr root) :
			currentGraph_(graph), root_(root) {}
		void execute() override { currentGraph_->updateMaxDist(root_); }
		void cancel() override {}
		void redo() override { currentGraph_->updateMaxDist(root_); }
	private:
		graph_sptr currentGraph_;
		node_sptr root_;
	};

	class AddGraph : public Command {
	public:
		AddGraph(GraphViewer* graphViewer, graph_sptr graphToAdd) :
			graphViewer_(graphViewer), graphToAdd_(graphToAdd) {}
		void execute() override { graphViewer_->addGraph(graphToAdd_); }
		void cancel() override { graphViewer_->removeGraph(graphToAdd_); }
		void redo() override { graphViewer_->addGraph(graphToAdd_); }
	private:
		GraphViewer* graphViewer_;
		graph_sptr graphToAdd_;
	};

	class ClearGraph : public Command {
	public:
		ClearGraph(GraphViewer* graphViewer, graph_sptr graphToClear, graph_sptr graphToSwapWith) :
			graphViewer_(graphViewer), graphToClear_(graphToClear), graphToSwapWith_(graphToSwapWith) {}
		void execute() override {
			graphViewer_->clearGraph(graphToSwapWith_);
		}
		void cancel() override {
			graphViewer_->clearGraph(graphToClear_);
		}
		void redo() override {
			graphViewer_->clearGraph(graphToSwapWith_);
		}
	private:
		GraphViewer* graphViewer_;
		graph_sptr graphToClear_;
		graph_sptr graphToSwapWith_;
	};

	class RemoveGraph : public Command {
	public:
		RemoveGraph(GraphViewer* graphViewer, graph_sptr graphToRemove) :
			addGraphCmd_(graphViewer, graphToRemove) {}
		void execute() override { addGraphCmd_.cancel(); }
		void cancel() override { addGraphCmd_.execute(); }
		void redo() override { addGraphCmd_.cancel(); }
	private:
		AddGraph addGraphCmd_; // RemoveGraph is inverse of AddGraph
	};

	class ChangeGraph : public Command { // should it be a cmd?? since we can't cancel...
	public:
		ChangeGraph(GraphViewer* graphViewer, graph_sptr graphToChange) :
			graphViewer_(graphViewer), graphToChange_(graphToChange) {}
		void execute() override { graphViewer_->changeGraph(graphToChange_); }
		void cancel() override {}
		void redo() override { graphViewer_->changeGraph(graphToChange_); }
	private:
		GraphViewer* graphViewer_;
		graph_sptr graphToChange_;
	};
}