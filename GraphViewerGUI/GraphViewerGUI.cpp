#include "GraphViewerGUI.hpp"

namespace View
{
    GraphViewerGUI::GraphViewerGUI(QWidget* parent)
        : QMainWindow(parent)
    {
        ui.setupUi(this);

        graphBoardScene_ = new QGraphicsScene(this);
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
        QObject::connect(ui.deleteNodeButton, SIGNAL(clicked()), this, SLOT(deleteNodeCmd()));
        QObject::connect(ui.connectNodesButton, SIGNAL(clicked()), this, SLOT(connectNodesCmd()));

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
            QObject::connect(graphToAdd.get(), &Model::Graph::nodeDeletedSignal, this, &GraphViewerGUI::deleteNodeView);
            QObject::connect(graphToAdd.get(), &Model::Graph::vertexAddedSignal, this, &GraphViewerGUI::connectNodesView);
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

    void GraphViewerGUI::changeGraphView(Model::graph_sptr graph) {
        // remake affichage of Node
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
                try {
                    invoker_->executeCommand(addNode);
                }
                catch (Model::SameName& error) {} // does not add the Node if it has the same name than one already existing
            }
        }
    }

    void GraphViewerGUI::addNodeView(Model::node_sptr node) {
        
        // creating the node and setting its position
        NodeGUI* nodeGUI = new NodeGUI(node);
        nodeGUI->setX(newNodePos_.x());
        nodeGUI->setY(newNodePos_.y());
        nodeGUI->setFlag(QGraphicsItem::ItemIsMovable);
        nodeGUI->setFlag(QGraphicsItem::ItemIsSelectable);
        
        // connect signals
        QObject::connect(nodeGUI, &NodeGUI::nodeReleased, this, &GraphViewerGUI::setNewNodePos);
        QObject::connect(nodeGUI, &NodeGUI::nodeSelected, this, &GraphViewerGUI::manageNodesSelection);

        graphBoardScene_->addItem(nodeGUI);
        currentGraphNodesGUI_.push_back(nodeGUI);
        
        // update the position of next Node
        verifyNodePos();
    }

    void GraphViewerGUI::deleteNodeCmd() {
        if (ui.nodeNameLineEdit->text() != "") {
            std::string name = ui.nodeNameLineEdit->text().toStdString();
            std::shared_ptr<Model::Command> removeNode = std::make_shared<Model::removeNode>(graphViewer_->getCurrentGraph(), getNodeGUI(name)->getNode());
            invoker_->executeCommand(removeNode);
        }
    }

    void GraphViewerGUI::deleteNodeView(Model::node_sptr node) {
        NodeGUI* nodeToDelete;
        for (int i = 0; i < currentGraphNodesGUI_.size(); i++) {
            if (currentGraphNodesGUI_[i]->getNode() == node) {
                nodeToDelete = currentGraphNodesGUI_[i];
                currentGraphNodesGUI_.erase(currentGraphNodesGUI_.begin() + i);
            }
        }
        graphBoardScene_->removeItem(nodeToDelete);
        if (nodeToDelete == previousFirstConnectedNode_) { previousFirstConnectedNode_ = nullptr; }
        else if (nodeToDelete == previousSecondConnectedNode_) { previousSecondConnectedNode_ = nullptr; }

        if (nodeToDelete->getNode()->getName() == ui.firstNodeSelectedLineEdit->text().toStdString()) {
            ui.firstNodeSelectedLineEdit->setText("");
        }
        else if (nodeToDelete->getNode()->getName() == ui.secondNodeSelectedLineEdit->text().toStdString()) {
            ui.secondNodeSelectedLineEdit->setText("");
        }
    }

    void GraphViewerGUI::manageNodesSelection(NodeGUI* nodeGUI) {
        ui.nodeNameLineEdit->setText(QString::fromStdString(nodeGUI->getNode()->getName()));
        ui.nodeValueSpinBox->setValue(nodeGUI->getNode()->getValue());
        switch (selectedNodeCount_)
        {
        case 0:
            selectedNodeCount_++;
            ui.firstNodeSelectedLineEdit->setText(QString::fromStdString(nodeGUI->getNode()->getName()));
            previousFirstConnectedNode_ = nodeGUI;
            break;
        case 1:
            selectedNodeCount_++;
            ui.secondNodeSelectedLineEdit->setText(QString::fromStdString(nodeGUI->getNode()->getName()));
            previousSecondConnectedNode_ = nodeGUI;
            break;
        case 2:
            selectedNodeCount_ = 1;
            ui.firstNodeSelectedLineEdit->setText(QString::fromStdString(nodeGUI->getNode()->getName()));
            ui.secondNodeSelectedLineEdit->setText("");
            if (previousSecondConnectedNode_) // to verify that it is not deleted
                previousSecondConnectedNode_->setBrush(QBrush(Qt::black));
            if (previousFirstConnectedNode_)
                previousFirstConnectedNode_->setBrush(QBrush(Qt::black));
            previousFirstConnectedNode_ = nodeGUI;
            break;
        default:
            break;
        }
        nodeGUI->setBrush(QBrush(Qt::red));
        update();
    }

    NodeGUI* GraphViewerGUI::getNodeGUI(std::string name) {
        for (auto&& nodeGUI : currentGraphNodesGUI_) {
            if (nodeGUI->getNode()->getName() == name) {
                return nodeGUI;
            }
        }
        return nullptr;
    }

    void GraphViewerGUI::setNewNodePos(NodeGUI* nodeGUI, int x, int y) {
        newNodePos_.setX(x); newNodePos_.setY(y);
        verifyNodePos();
        std::vector<Model::vertex_sptr> neighbourVertices = graphViewer_->getCurrentGraph()->getVerticesNeighbours()[nodeGUI->getNode()];
        std::vector<Model::node_sptr> parentNodes = graphViewer_->getCurrentGraph()->getParentNodes()[nodeGUI->getNode()];
        NodeGUI* secondNode;
        for (auto&& vertex : neighbourVertices) {
            VertexGUI* vertexGUI = findVertexGUI(vertex);
            if (vertexGUI) {
                //NodeGUI* firstNode = getNodeGUI(vertex->getPreviousNode()->getName());
                // the firstNode GUI is the current one, nodeGUI (the parent)
                secondNode = getNodeGUI(vertex->getNode()->getName());

                vertexGUI->setLine(nodeGUI->x(), nodeGUI->y(), secondNode->x(), secondNode->y());
            }
        }
        for (auto&& node : parentNodes) {
            for (auto&& vertex : graphViewer_->getCurrentGraph()->findVertices(node, nodeGUI->getNode())) {
                VertexGUI* verterxGUIParents = findVertexGUI(vertex);
                if (verterxGUIParents) {
                    NodeGUI* nodeGUIParent = getNodeGUI(node->getName());
                    verterxGUIParents->setLine(nodeGUIParent->x(), nodeGUIParent->y(), nodeGUI->x(), nodeGUI->y());
                }
            }
        }
    }

    void GraphViewerGUI::verifyNodePos() {
        bool goodPos = false;
        int countOfLoop = 0;
        //infinite loop if there is no place
        while (!goodPos) {
            int direction = std::rand() % 3; // 0 = left, 1 = right, 2 = up, default = down
            switch (direction) {
            case 0:
                newNodePos_.setX(newNodePos_.x() -20);
                break;
            case 1:
                if (newNodePos_.x() + 30 < graphBoardView_->size().width()) {
                    newNodePos_.setX(newNodePos_.x() + 20);
                    break;
                }
            case 2:
                if (newNodePos_.x() + 30 < graphBoardView_->size().height()) {
                    newNodePos_.setY(newNodePos_.y() + 20);
                    break;
                }
            default:
                newNodePos_.setY(newNodePos_.y() - 20);
                break;
            }

            // I have to use this inefficient method because graphBoardView_itemAt(pos) does not work for some reason.
            QList<QGraphicsItem*> allNodes = graphBoardView_->items();
            for (int i = 0; i < allNodes.size(); i++) {
                if (newNodePos_.x() == allNodes[i]->x() && newNodePos_.y() == allNodes[i]->y()) {
                    break;
                }
                else if (i == allNodes.size() - 1) {
                    goodPos = true;
                }
            }
            countOfLoop++;
            if (countOfLoop == 50) {
                // throw error
            }
        }
    }

    VertexGUI* GraphViewerGUI::findVertexGUI(Model::vertex_sptr vertex) {
        for (auto&& existingVertex : currentGraphVerticesGUI_) {
            if (existingVertex->getVertex() == vertex) {
                return existingVertex;
            }
        }
        return nullptr;
    }

    void GraphViewerGUI::connectNodesCmd() {
        std::string firstNodeName = ui.firstNodeSelectedLineEdit->text().toStdString();
        std::string secondNodeName = ui.secondNodeSelectedLineEdit->text().toStdString();
        int weight = ui.vertexWeightSpinBox->value();
        if (firstNodeName != "" && secondNodeName != "") {
            std::shared_ptr<Model::Command> connectNodes;
            connectNodes = std::make_shared<Model::ConnectNodes>(graphViewer_->getCurrentGraph(), 
                getNodeGUI(firstNodeName)->getNode(), getNodeGUI(secondNodeName)->getNode(), weight);
            try {
                invoker_->executeCommand(connectNodes);
            }
            catch (Model::SameName& error) {}
        }
    }

    void GraphViewerGUI::connectNodesView(Model::vertex_sptr vertex) {
        /*
        // creating the node and setting its position
        NodeGUI* nodeGUI = new NodeGUI(node);
        nodeGUI->setX(newNodePos_.x());
        nodeGUI->setY(newNodePos_.y());
        nodeGUI->setFlag(QGraphicsItem::ItemIsMovable);
        nodeGUI->setFlag(QGraphicsItem::ItemIsSelectable);
        
        // connect signals
        QObject::connect(nodeGUI, &NodeGUI::nodeReleased, this, &GraphViewerGUI::setNewNodePos);
        QObject::connect(nodeGUI, &NodeGUI::nodeSelected, this, &GraphViewerGUI::manageNodesSelection);

        graphBoardScene_->addItem(nodeGUI);
        currentGraphNodesGUI_.push_back(nodeGUI);
        
        // update the position of next Node
        verifyNodePos();
        */

        // creating the vertex and setting up the position
        NodeGUI* firstNode = getNodeGUI(vertex->getPreviousNode()->getName());
        NodeGUI* secondNode = getNodeGUI(vertex->getNode()->getName());
        VertexGUI* vertexGUI = new VertexGUI(vertex, firstNode, secondNode);

        // I set the line in the constructor
        //vertexGUI->setLine(firstNode->x(), firstNode->y(), secondNode->x(), secondNode->y());
        vertexGUI->setFlag(QGraphicsItem::ItemIsSelectable);

        // connect signals
        //QObject::connect(nodeGUI, &NodeGUI::nodeSelected, this, &GraphViewerGUI::manageNodesSelection);
        graphBoardScene_->addItem(vertexGUI);
        currentGraphVerticesGUI_.push_back(vertexGUI);
    }

}