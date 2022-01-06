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

    public slots:
        void addNodeCmd();
        void addNodeView(Model::node_sptr node);
        void deleteNodeCmd();
        void deleteNodeView(Model::node_sptr node);
        void manageNodesSelection(NodeGUI* nodeGUI);

        void addGraphCmd();
        void addGraphView(Model::graph_sptr graph);
        void changeGraphCmd(QListWidgetItem* current, QListWidgetItem* previous);
        void changeGraphView(Model::graph_sptr graph);
        void clearGraphCmd();
        void clearGraphView();

        void connectNodesCmd();
        void connectNodesView(Model::vertex_sptr vertex);
        void deleteVertexCmd();
        void deleteVertexView(Model::vertex_sptr vertex);
        void updateSelectedVertex(VertexGUI* vertex, bool isSelected);

        void updateMinDistsCmd();
        void updateMaxDistsCmd();
        void updateMinDistsView();
        void updateMaxDistsView();

        void clearDistsItem();


        void setNewNodePos(NodeGUI* node, int x, int y);
        void verifyNodePos();

    private:
        Ui::GraphViewerGUIClass ui;
        Model::Invoker* invoker_ = Model::Invoker::getInvoker();
        std::unordered_map<std::string, Model::graph_sptr> graphsMap_; 

        // Not use this anymore
        // std::unordered_map<std::string, NodeGUI*> currentGraphNodesGUI_;
        //std::unordered_map<Model::vertex_sptr, VertexGUI*> currentGraphVerticesGUI_;
        
        std::unordered_map<std::string, std::unordered_map<std::string, NodeGUI*>> mapGraphsNodesGUI_;
        std::unordered_map<std::string, std::unordered_map<Model::vertex_sptr, VertexGUI*>> mapGraphsVerticesGUI_;

        NodeGUI* previousFirstConnectedNode_ = nullptr;
        NodeGUI* previousSecondConnectedNode_ = nullptr;

        VertexGUI* selectedVertex_ = nullptr;

        Model::GraphViewer* graphViewer_ = Model::GraphViewer::getGraphViewer();
        QGraphicsScene* graphBoardScene_;
        QGraphicsView* graphBoardView_;
        QPoint newNodePos_;

        int selectedNodeCount_ = 0; // to know the first and second Node Selected
    };
}
