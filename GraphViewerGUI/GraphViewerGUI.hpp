#pragma once

#include "Invoker.hpp"
#include "GraphViewer.hpp"
#include "Graph.hpp"
#include "NodeGUI.hpp"
#include "VertexGUI.hpp"
#include <queue>

#pragma warning(push, 0) // remove the useless warning of Qt?
#include <QtWidgets/QMainWindow>
#include "qcheckbox.h"
#include "ui_GraphViewerGUI.h"
#include "qgraphicsview.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem.h"
#include "qshortcut.h"
#pragma pop()

namespace View
{
    class GraphViewerGUI : public QMainWindow
    {
        Q_OBJECT

    public:
        GraphViewerGUI(QWidget* parent = Q_NULLPTR);
        void initialize();
        void setUpConnections();
        void clearGUI();
        void changeCurrentGraphView(std::string graphName);
        void verifyCurrentGraph(Model::Graph* graph); // make sure that we are updating the view of the currentGraph

    public slots:
        void addNodeCmd();
        void addNodeView(Model::Graph* graph, Model::node_sptr node);
        void deleteNodeCmd();
        void deleteNodeView(Model::Graph* graph, Model::node_sptr node);
        void manageNodesSelection(NodeGUI* nodeGUI);

        void addGraphCmd();
        void addGraphView(Model::graph_sptr graph);
        void changeGraphCmd(QListWidgetItem* current, QListWidgetItem* previous);
        void changeGraphView(Model::graph_sptr graph);
        void clearGraphCmd();
        void clearGraphView(Model::Graph*);
        void removeGraphCmd();
        void removeGraphView(Model::graph_sptr);

        void connectNodesCmd();
        void connectNodesView(Model::Graph* graph, Model::vertex_sptr vertex);
        void deleteVertexCmd();
        void deleteVertexView(Model::Graph* graph, Model::vertex_sptr vertex);
        void updateSelectedVertex(VertexGUI* vertex, bool isSelected);

        void updateMinDistsCmd();
        void updateMaxDistsCmd();
        void updateMinDistsView(Model::Graph* graph);
        void updateMaxDistsView(Model::Graph* graph);

        void clearDistsItem();

        void setNewNodePos(NodeGUI* node, int x, int y);
        void verifyNodePos();

        void cancelCmd();
        void redoCmd();

    private:
        Ui::GraphViewerGUIClass ui;
        Model::Invoker* invoker_ = Model::Invoker::getInvoker();
        std::unordered_map<std::string, Model::graph_sptr> graphsMap_; 

        std::unordered_map<std::string, std::unordered_map<std::string, NodeGUI*>> mapGraphsNodesGUI_;
        std::unordered_map<std::string, std::unordered_map<Model::vertex_sptr, VertexGUI*>> mapGraphsVerticesGUI_;

        NodeGUI* previousFirstConnectedNode_ = nullptr;
        NodeGUI* previousSecondConnectedNode_ = nullptr;

        VertexGUI* selectedVertex_ = nullptr;

        Model::GraphViewer* graphViewer_ = Model::GraphViewer::getGraphViewer();
        QGraphicsScene* graphBoardScene_ = nullptr;
        QGraphicsView* graphBoardView_ = nullptr;
        QPoint newNodePos_;

        int selectedNodeCount_ = 0; // to know the first and second Node Selected
    };
}
