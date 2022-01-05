#pragma once

#include "Invoker.hpp"
#include "GraphViewer.hpp"
#include "Graph.hpp"

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

    public slots:
        void addNodeCmd();
        void addNodeView(Model::node_sptr node);

        void addGraphCmd();
        void addGraphView(Model::graph_sptr graph);

        void changeGraphView(Model::graph_sptr graph);
    private:
        Ui::GraphViewerGUIClass ui;
        Model::Invoker* invoker_ = Model::Invoker::getInvoker();
        std::vector<Model::graph_sptr> graphsList_;

        Model::GraphViewer* graphViewer_ = Model::GraphViewer::getGraphViewer();
        QGraphicsScene* graphBoardScene_;
        QGraphicsView* graphBoardView_;
        QPoint newNodePos_;
    };
}
