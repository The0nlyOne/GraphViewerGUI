#pragma once

#include "Invoker.hpp"
#include "GraphViewer.hpp"
#include "Graph.hpp"
#include "NodeGUI.hpp"
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

        Model::node_sptr getNode(std::string name);

    public slots:
        void addNodeCmd();
        void addNodeView(Model::node_sptr node);
        void deleteNodeCmd();
        void deleteNodeView(Model::node_sptr node);
        void manageNodesSelection(NodeGUI* nodeGUI);

        void addGraphCmd();
        void addGraphView(Model::graph_sptr graph);

        void changeGraphView(Model::graph_sptr graph);
        
        void setNewNodePos(int x, int y);
        void verifyNodePos();

    private:
        Ui::GraphViewerGUIClass ui;
        Model::Invoker* invoker_ = Model::Invoker::getInvoker();
        std::vector<Model::graph_sptr> graphsList_;
        std::vector<NodeGUI*> currentGraphNodesGUI_;
        NodeGUI* previousFirstConnectedNode_ = nullptr;
        NodeGUI* previousSecondConnectedNode_ = nullptr;

        Model::GraphViewer* graphViewer_ = Model::GraphViewer::getGraphViewer();
        QGraphicsScene* graphBoardScene_;
        QGraphicsView* graphBoardView_;
        QPoint newNodePos_;

        int selectedNodeCount_ = 0; // to know the first and second Node Selected
    };
}
