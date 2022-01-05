#include "GraphViewerGUI.hpp"
#include "NodeGUI.hpp"

namespace View
{
    GraphViewerGUI::GraphViewerGUI(QWidget* parent)
        : QMainWindow(parent)
    {
        ui.setupUi(this);

        graphBoardScene_ = new QGraphicsScene();
        graphBoardView_ = ui.graphBoardGraphicsView;
        graphBoardView_->setScene(graphBoardScene_);

        QPoint newNodePos(0, 0);
        newNodePos_ = newNodePos;

    }

    void GraphViewerGUI::initialize()
    {
        // graphBoardScene_->addEllipse(10, 10, 20, 20);

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
        
        // creating the node and setting its position
        NodeGUI* nodeGUI = new NodeGUI(node);
        nodeGUI->setX(newNodePos_.x());
        nodeGUI->setY(newNodePos_.y());
        graphBoardScene_->addItem(nodeGUI);
        
        // update the position of next Node
        if (newNodePos_.x() + 40 < graphBoardView_->size().width()) {
            newNodePos_.setX(newNodePos_.x() + 40);
        }
        else if (newNodePos_.x() + 40 > graphBoardView_->size().width() && newNodePos_.y() + 40 < graphBoardView_->size().height()) {
            newNodePos_.setY(newNodePos_.y() + 40);
        }
        else if (newNodePos_.x() + 40 > graphBoardView_->size().width() && newNodePos_.y() + 40 > graphBoardView_->size().height()) {
            newNodePos_.setX(newNodePos_.x() - 40);
        }
    }

    void GraphViewerGUI::changeGraphView(Model::graph_sptr graph) {
        // remake affichage of Node
    }
}