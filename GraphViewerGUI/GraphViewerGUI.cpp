#include "GraphViewerGUI.hpp"

namespace View
{
    GraphViewerGUI::GraphViewerGUI(QWidget* parent)
        : QMainWindow(parent)
    {
        ui.setupUi(this);
        QPoint newNodePos(0, 0);
        newNodePos_ = newNodePos;

    }

    void GraphViewerGUI::initialize()
    {
        graphBoardScene_ = new QGraphicsScene();
        //graphBoardScene_->setSceneRect(QRect(0,0,1920,1080));
        graphBoardScene_->addEllipse(10, 10, 20, 20);
        graphBoardView_ = ui.graphBoardGraphicsView;
        graphBoardView_->setScene(graphBoardScene_);
        setUpConnections();
        //graphBoardView_->show();
    }
    
    void GraphViewerGUI::setUpConnections() {

        //connect the buttons
        QObject::connect(ui.addGraphButton, SIGNAL(clicked()), this, SLOT(addGraphCmd()));
        QObject::connect(ui.addNodeButton, SIGNAL(clicked()), this, SLOT(addNodeCmd()));

        // connect the model to the view
        QObject::connect(graphViewer_, &Model::GraphViewer::graphAddedSignal, this, &GraphViewerGUI::addGraphView);
        //QObject::connect(graphViewer_, &Model::GraphViewer::graphRemovedSignal, this, &GraphViewerGUI::removeProfileAction);
        QObject::connect(graphViewer_, &Model::GraphViewer::graphChangedSignal, this, &GraphViewerGUI::changeGraphView);
    }

    void GraphViewerGUI::addGraphCmd()
    {
        if (ui.graphNamelineEdit->text() != "") {
            std::string name = ui.graphNamelineEdit->text().toStdString();
            Model::graph_sptr graphToAdd = std::make_shared<Model::Graph>(name);
            std::shared_ptr<Model::Command> addGraph = std::make_shared<Model::AddGraph>(graphViewer_, graphToAdd);

            QObject::connect(graphToAdd.get(), &Model::Graph::nodeAddedSignal, this, &GraphViewerGUI::addNodeView);
            //QObject::connect(graphToAdd.get(), &Model::Graph::nodeRemovedSignal, this, &GraphViewerGUI::removeNodeView);
            //QObject::connect(graphToAdd.get(), &Model::Graph::nodeShowed, this, &GraphViewerGUI::updateBudgetAction);

            try {
                invoker_->executeCommand(addGraph);
                if (ui.graphsListWidget->count() == 1) {// if it is the first graph
                    std::shared_ptr<Model::Command> changeGraph = std::make_shared<Model::ChangeGraph>(graphViewer_, graphToAdd);
                    invoker_->executeCommand(changeGraph);
                }
            }
            catch (Model::SameName& error) {} // does not add the graph if it has the same name than one already existing
        }
    }

    void GraphViewerGUI::addGraphView(Model::graph_sptr graph) {
        ui.graphsListWidget->addItem(QString::fromStdString(graph->getName()));
    }

    void GraphViewerGUI::addNodeCmd() {
        // Verify that there can't be 2 Node with same name here.
        if (graphViewer_->getCurrentGraph() != nullptr) {
            if (ui.nodeNameLineEdit->text() != "") {
                // getting needed info
                std::string name = ui.nodeNameLineEdit->text().toStdString();
                int value = ui.nodeValueSpinBox->value();

                Model::node_sptr nodeToAdd = std::make_shared<Model::Node>(name, value);
                std::shared_ptr<Model::Command> addNode = std::make_shared<Model::AddNode>(graphViewer_->getCurrentGraph(), nodeToAdd);

                invoker_->executeCommand(addNode);
            }
        }
    }

    void GraphViewerGUI::addNodeView(Model::node_sptr node) {
        
        QWidget* previousWidget = ui.graphScrollArea->widget();

        // creating new Widget
        QWidget* graphBoardContent = new QWidget();
        graphBoardContent->setObjectName(QString::fromUtf8("graphBoardContents"));
        graphBoardContent->setGeometry(QRect(0, 0, 1194, 754));

        // creating the node
        QCheckBox* nodeView = new QCheckBox(graphBoardContent);
        nodeView->setObjectName(QString::fromStdString(node->getName()));
        nodeView->setGeometry(QRect(newNodePos_.x(), newNodePos_.y(), 17, 17));

        // copying the previous node to the new node
        for (auto&& widget : previousWidget->children()) {
            QCheckBox* previousNode = dynamic_cast<QCheckBox*>(widget);
            QCheckBox* previousNodeCopy = new QCheckBox(graphBoardContent);
            previousNodeCopy->setObjectName(previousNode->objectName());
            previousNodeCopy->setGeometry(QRect(previousNode->x(), previousNode->y(), 17, 17));
            previousNodeCopy->setParent(graphBoardContent);
        }

        // updating the widget
        ui.graphScrollArea->setWidget(graphBoardContent);

        // update the position of next Node
        if (newNodePos_.x() + 40 < graphBoardContent->size().width()) {
            newNodePos_.setX(newNodePos_.x() + 40);
        }
        else if (newNodePos_.x() + 40 > graphBoardContent->size().width() && newNodePos_.y() + 40 < graphBoardContent->size().height()) {
            newNodePos_.setY(newNodePos_.y() + 40);
        }
        else if (newNodePos_.x() + 40 > graphBoardContent->size().width() && newNodePos_.y() + 40 > graphBoardContent->size().height()) {
            newNodePos_.setX(newNodePos_.x() - 40);
        }
        
    }

    void GraphViewerGUI::changeGraphView(Model::graph_sptr graph) {
        // remake affichage of Node
    }
}