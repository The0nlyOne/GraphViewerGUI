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
        setUpConnections();
    }
    
    void GraphViewerGUI::setUpConnections() {

        //connect the buttons
        QObject::connect(ui.addGraphButton, SIGNAL(clicked()), this, SLOT(addGraphCmd()));
        QObject::connect(ui.addNodeButton, SIGNAL(clicked()), this, SLOT(addNodeCmd()));
        QObject::connect(ui.deleteNodeButton, SIGNAL(clicked()), this, SLOT(deleteNodeCmd()));
        QObject::connect(ui.connectNodesButton, SIGNAL(clicked()), this, SLOT(connectNodesCmd()));
        QObject::connect(ui.deleteVertexButton, SIGNAL(clicked()), this, SLOT(deleteVertexCmd()));
        QObject::connect(ui.updateMinDistButton, SIGNAL(clicked()), this, SLOT(updateMinDistsCmd()));
        QObject::connect(ui.updateMaxDistButton, SIGNAL(clicked()), this, SLOT(updateMaxDistsCmd()));
        QObject::connect(ui.clearDistsValuesButton, SIGNAL(clicked()), this, SLOT(clearDistsItem()));
        QObject::connect(ui.clearGraphButton, SIGNAL(clicked()), this, SLOT(clearGraphCmd()));
        QObject::connect(ui.deleteGraphButton, SIGNAL(clicked()), this, SLOT(removeGraphCmd()));
        QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancelCmd()));
        QObject::connect(ui.redoButton, SIGNAL(clicked()), this, SLOT(redoCmd()));

        // listConnection
        QObject::connect(ui.graphsListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(changeGraphCmd(QListWidgetItem*, QListWidgetItem*)));

        // connect the model to the view
        QObject::connect(graphViewer_, &Model::GraphViewer::graphAddedSignal, this, &GraphViewerGUI::addGraphView);
        QObject::connect(graphViewer_, &Model::GraphViewer::graphRemovedSignal, this, &GraphViewerGUI::removeGraphView);
        QObject::connect(graphViewer_, &Model::GraphViewer::graphChangedSignal, this, &GraphViewerGUI::changeGraphView);
        QObject::connect(graphViewer_, &Model::GraphViewer::graphCleared, this, &GraphViewerGUI::clearGraphView);

        // second shortcut for redoButton. The first one was added with the Qt editor
        QShortcut* redoShortcut2 = new QShortcut(QKeySequence("Ctrl+Y"), this);
        QObject::connect(redoShortcut2, SIGNAL(activated()), ui.redoButton, SLOT(click()));
    }

    void GraphViewerGUI::connectGraph(Model::Graph* graph) {
        QObject::connect(graph, &Model::Graph::nodeAddedSignal, this, &GraphViewerGUI::addNodeView);
        QObject::connect(graph, &Model::Graph::nodeDeletedSignal, this, &GraphViewerGUI::deleteNodeView);
        QObject::connect(graph, &Model::Graph::vertexAddedSignal, this, &GraphViewerGUI::connectNodesView);
        QObject::connect(graph, &Model::Graph::vertexDeletedSignal, this, &GraphViewerGUI::deleteVertexView);
        QObject::connect(graph, &Model::Graph::minDistUpdatedSignal, this, &GraphViewerGUI::updateMinDistsView);
        QObject::connect(graph, &Model::Graph::maxDistUpdatedSignal, this, &GraphViewerGUI::updateMaxDistsView);
        //QObject::connect(graphToAdd.get(), &Model::Graph::graphCleared, this, &GraphViewerGUI::clearGraphView);
    }

    void GraphViewerGUI::addGraphCmd()
    {
        if (ui.graphNamelineEdit->text() == "") { return; }

        std::string name = ui.graphNamelineEdit->text().toStdString();

        if (graphsMap_[name]) { return; } // does not try to add an already existing graph

        Model::graph_sptr graphToAdd = std::make_shared<Model::Graph>(name);
        std::shared_ptr<Model::Command> addGraph = std::make_shared<Model::AddGraph>(graphViewer_, graphToAdd);

        connectGraph(graphToAdd.get());
        /*
        QObject::connect(graphToAdd.get(), &Model::Graph::nodeAddedSignal, this, &GraphViewerGUI::addNodeView);
        QObject::connect(graphToAdd.get(), &Model::Graph::nodeDeletedSignal, this, &GraphViewerGUI::deleteNodeView);
        QObject::connect(graphToAdd.get(), &Model::Graph::vertexAddedSignal, this, &GraphViewerGUI::connectNodesView);
        QObject::connect(graphToAdd.get(), &Model::Graph::vertexDeletedSignal, this, &GraphViewerGUI::deleteVertexView);
        QObject::connect(graphToAdd.get(), &Model::Graph::minDistUpdatedSignal, this, &GraphViewerGUI::updateMinDistsView);
        QObject::connect(graphToAdd.get(), &Model::Graph::maxDistUpdatedSignal, this, &GraphViewerGUI::updateMaxDistsView);
        //QObject::connect(graphToAdd.get(), &Model::Graph::graphCleared, this, &GraphViewerGUI::clearGraphView);
        */

        invoker_->executeCommand(addGraph);
    }

    void GraphViewerGUI::addGraphView(Model::graph_sptr graph) {
        std::string graphName = graph->getName();
        ui.graphsListWidget->addItem(QString::fromStdString(graphName));
        graphsMap_[graphName] = graph;
        /*
        QListWidget* graphsList = ui.graphsListWidget;
        if (graphsList->count() == 1) { // set the first graph as current if there are no graph
            graphsList->setCurrentItem(graphsList->item(0));
        }
        */
        changeCurrentGraphView(graphName);

        ui.graphNamelineEdit->setText("");
    }

    void GraphViewerGUI::changeGraphCmd(QListWidgetItem* current, QListWidgetItem* previous) {
        /*
        if (current)
        {
            std::string name = current->text().toStdString();

            std::shared_ptr<Model::Command> changeGraph = std::make_shared<Model::ChangeGraph>(graphViewer_, graphsMap_[name]);
            invoker_->executeCommand(changeGraph);
        }
        else
        {
            std::shared_ptr<Model::Command> changeGraph = std::make_shared<Model::ChangeGraph>(graphViewer_, nullptr);
            invoker_->executeCommand(changeGraph);
        }
        */
        if (current) {
            std::string graphName = current->text().toStdString();
            graphViewer_->changeGraph(graphsMap_[graphName]); // will emit the graphChanged signal
        }
        else {
            clearGUI();
        }
    }

    void GraphViewerGUI::changeGraphView(Model::graph_sptr graph) {
        clearGUI();

        if (!graph) { return; }

        std::string graphName = graph->getName();
        if (mapGraphsNodesGUI_[graphName].size() >= 1) {
            for (auto&& pairNameAndNodeGUI : mapGraphsNodesGUI_[graphName]) {
                NodeGUI* nodeGUI = pairNameAndNodeGUI.second;
                nodeGUI->setBrush(QBrush(Qt::darkCyan));
                graphBoardScene_->addItem(nodeGUI);
                graphBoardScene_->addItem(nodeGUI->getNameGUI());
                graphBoardScene_->addItem(nodeGUI->getDistsGUI());
            }
        }

        if (mapGraphsVerticesGUI_[graphName].size() >= 1) {
            for (auto&& pairVertexSptrAndVertexGUI : mapGraphsVerticesGUI_[graphName]) {
                graphBoardScene_->addItem(pairVertexSptrAndVertexGUI.second);
            }
        }
        // load previous graph info
    }

    void GraphViewerGUI::clearGraphCmd() {
        // it is not a command so we can't cancel
        /*
        if (graphViewer_->getCurrentGraph()) {
            graphViewer_->getCurrentGraph()->clearGraph();
        }

        if (graphsMap_[name]) { return; } // does not try to add an already existing graph

        Model::graph_sptr graphToAdd = std::make_shared<Model::Graph>(name);
        std::shared_ptr<Model::Command> addGraph = std::make_shared<Model::AddGraph>(graphViewer_, graphToAdd);

        QObject::connect(graphToAdd.get(), &Model::Graph::nodeAddedSignal, this, &GraphViewerGUI::addNodeView);
        QObject::connect(graphToAdd.get(), &Model::Graph::nodeDeletedSignal, this, &GraphViewerGUI::deleteNodeView);
        QObject::connect(graphToAdd.get(), &Model::Graph::vertexAddedSignal, this, &GraphViewerGUI::connectNodesView);
        QObject::connect(graphToAdd.get(), &Model::Graph::vertexDeletedSignal, this, &GraphViewerGUI::deleteVertexView);
        QObject::connect(graphToAdd.get(), &Model::Graph::minDistUpdatedSignal, this, &GraphViewerGUI::updateMinDistsView);
        QObject::connect(graphToAdd.get(), &Model::Graph::maxDistUpdatedSignal, this, &GraphViewerGUI::updateMaxDistsView);
        */

        Model::graph_sptr currentGraph = graphViewer_->getCurrentGraph();
        if (!currentGraph) { return; }

        std::string name = currentGraph->getName();
        Model::graph_sptr graphToSwapWith = std::make_shared<Model::Graph>(name);
        std::shared_ptr<Model::Command> clearGraph = std::make_shared<Model::ClearGraph>(graphViewer_, currentGraph, graphToSwapWith);

        connectGraph(graphToSwapWith.get());

        invoker_->executeCommand(clearGraph);
    }

    
    void GraphViewerGUI::clearGraphView(Model::graph_sptr graph) {
        // it is not a command so we can't cancel
        clearGUI();

        if (!graph) { return; }

        // do not erase cause we still use the graphname
        mapGraphsNodesGUI_[graph->getName()].clear();
        mapGraphsVerticesGUI_[graph->getName()].clear();

        // add current graph nodeGUI and vertices GUI
        for (auto&& node : graph->getNodes()) {
            addNodeView(graph.get(), node);
        }
        for (auto&& vertex : graph->getVertices()) {
            connectNodesView(graph.get(), vertex);
        }
    }
    

    void GraphViewerGUI::clearGUI() {
        ui.nodeNameLineEdit->setText("");
        ui.nodeValueSpinBox->setValue(0);
        ui.firstNodeSelectedLineEdit->setText("");
        ui.secondNodeSelectedLineEdit->setText("");
        ui.vertexWeightSpinBox->setValue(1);
        ui.vertexSelectedLineEdit->setText("");
        ui.rootGraphLineEdit->setText("");

        selectedNodeCount_ = 0;
        selectedVertex_ = nullptr;

        NodeGUI* previousFirstConnectedNode_ = nullptr;
        NodeGUI* previousSecondConnectedNode_ = nullptr;

        QList<QGraphicsItem*> listGUI = graphBoardScene_->items();
        for (auto&& item : listGUI) {
            graphBoardScene_->removeItem(item); // does it delete the ptr or just remove from board?
        }
    }

    void GraphViewerGUI::removeGraphCmd() {
        if (!graphViewer_->getCurrentGraph()) { return; }
        std::shared_ptr<Model::Command> removeGraph = std::make_shared<Model::RemoveGraph>(graphViewer_, graphViewer_->getCurrentGraph());
        invoker_->executeCommand(removeGraph);
    }

    void GraphViewerGUI::removeGraphView(Model::graph_sptr graph) {
        if (!graph) { return; }
        for (int i = 0; i < ui.graphsListWidget->count(); i++) {
            if (ui.graphsListWidget->item(i)->text().toStdString() == graph->getName()) {
                ui.graphsListWidget->takeItem(i);
                graphsMap_.erase(graph->getName());
                break;
            }
        }
    }

    void GraphViewerGUI::changeCurrentGraphView(std::string graphName) {
        for (int i = 0; i < ui.graphsListWidget->count(); i++) {
            if (ui.graphsListWidget->item(i)->text().toStdString() == graphName) {
                ui.graphsListWidget->setCurrentItem(ui.graphsListWidget->item(i)); // will send a signal that will trigger changeGraphCmd()
                break;
            }
        }
    }

    void GraphViewerGUI::verifyCurrentGraph(Model::Graph* graph) {
        if (graph->getName() != graphViewer_->getCurrentGraph()->getName()) {
            changeCurrentGraphView(graph->getName()); 
        }
    }

    void GraphViewerGUI::addNodeCmd() {
        // Verify that there can't be 2 Node with same name here.
        if (!graphViewer_->getCurrentGraph()) { return; }

        if (ui.nodeNameLineEdit->text() != "") {
            // getting needed info
            std::string name = ui.nodeNameLineEdit->text().toStdString();
            int value = ui.nodeValueSpinBox->value();

            Model::node_sptr nodeToAdd = std::make_shared<Model::Node>(name, value);
            std::shared_ptr<Model::Command> addNode = std::make_shared<Model::AddNode>(graphViewer_->getCurrentGraph(), nodeToAdd);
            invoker_->executeCommand(addNode);
        }
    }

    void GraphViewerGUI::addNodeView(Model::Graph* graph, Model::node_sptr node) {
        // creating the node and setting its position
        NodeGUI* nodeGUI = new NodeGUI(node);
        nodeGUI->setX(newNodePos_.x());
        nodeGUI->setY(newNodePos_.y());
        nodeGUI->setFlag(QGraphicsItem::ItemIsMovable);
        nodeGUI->setFlag(QGraphicsItem::ItemIsSelectable);
        
        // creating text
        QGraphicsTextItem* nodeNameGUI = new QGraphicsTextItem(QString::fromStdString(node->getName()));
        nodeGUI->setNameGUI(nodeNameGUI);
        nodeNameGUI->setPos(nodeGUI->x() + nodeGUI->getNodeNameDist(), nodeGUI->y());

        // connect signals
        QObject::connect(nodeGUI, &NodeGUI::nodeReleased, this, &GraphViewerGUI::setNewNodePos);
        QObject::connect(nodeGUI, &NodeGUI::nodeSelected, this, &GraphViewerGUI::manageNodesSelection);

        mapGraphsNodesGUI_[graph->getName()][node->getName()] = nodeGUI;

        verifyCurrentGraph(graph);
        graphBoardScene_->addItem(nodeGUI);
        graphBoardScene_->addItem(nodeNameGUI);
        ui.nodeNameLineEdit->setText("");

        // update the position of next Node
        verifyNodePos();
    }

    void GraphViewerGUI::deleteNodeCmd() {
        if (ui.nodeNameLineEdit->text() == "") { return; }

        std::string name = ui.nodeNameLineEdit->text().toStdString();
        NodeGUI* nodeToDelete = mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()][name];
        if (nodeToDelete) {
            std::shared_ptr<Model::Command> removeNode = std::make_shared<Model::removeNode>(graphViewer_->getCurrentGraph(), nodeToDelete->getNode());
            invoker_->executeCommand(removeNode);
        }
    }

    void GraphViewerGUI::deleteNodeView(Model::Graph* graph, Model::node_sptr node) {
        verifyCurrentGraph(graph);
        NodeGUI* nodeToDelete = mapGraphsNodesGUI_[graph->getName()][node->getName()];
        mapGraphsNodesGUI_[graph->getName()].erase(node->getName());
        graphBoardScene_->removeItem(nodeToDelete);

        if (nodeToDelete == previousFirstConnectedNode_) { previousFirstConnectedNode_ = nullptr; }
        else if (nodeToDelete == previousSecondConnectedNode_) { previousSecondConnectedNode_ = nullptr; }

        if (nodeToDelete->getNode()->getName() == ui.firstNodeSelectedLineEdit->text().toStdString()) {
            ui.firstNodeSelectedLineEdit->setText("");
        }
        else if (nodeToDelete->getNode()->getName() == ui.secondNodeSelectedLineEdit->text().toStdString()) {
            ui.secondNodeSelectedLineEdit->setText("");
        }

        ui.nodeNameLineEdit->setText("");
        ui.rootGraphLineEdit->setText("");
        ui.nodeValueSpinBox->setValue(0);

        delete nodeToDelete; // the nameGUI and distGUI will be deleted by the class
    }

    void GraphViewerGUI::manageNodesSelection(NodeGUI* nodeGUI) {
        if (!nodeGUI) { return; }

        ui.nodeNameLineEdit->setText(QString::fromStdString(nodeGUI->getNode()->getName()));
        ui.rootGraphLineEdit->setText(QString::fromStdString(nodeGUI->getNode()->getName()));
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
                previousSecondConnectedNode_->setBrush(QBrush(Qt::darkCyan));
            if (previousFirstConnectedNode_)
                previousFirstConnectedNode_->setBrush(QBrush(Qt::darkCyan));
            previousFirstConnectedNode_ = nodeGUI;
            break;
        default:
            break;
        }
        nodeGUI->setBrush(QBrush(Qt::red));
        update();
    }

    void GraphViewerGUI::setNewNodePos(NodeGUI* nodeGUI, int x, int y) {
        newNodePos_.setX(x); newNodePos_.setY(y);
        nodeGUI->getNameGUI()->setPos(nodeGUI->x() + nodeGUI->getNodeNameDist(), nodeGUI->y());
        if (nodeGUI->getDistsGUI()) {
            nodeGUI->getDistsGUI()->setPos(nodeGUI->x() + nodeGUI->getNodeNameDist(), nodeGUI->y() - nodeGUI->getNodeNameDist());
        }
        verifyNodePos();

        std::vector<Model::vertex_sptr> neighbourVertices = graphViewer_->getCurrentGraph()->getVerticesNeighbours()[nodeGUI->getNode()];
        std::vector<Model::node_sptr> parentNodes = graphViewer_->getCurrentGraph()->getParentNodes()[nodeGUI->getNode()];
        NodeGUI* secondNode;
        for (auto&& vertex : neighbourVertices) {
            VertexGUI* vertexGUI = mapGraphsVerticesGUI_[graphViewer_->getCurrentGraph()->getName()][vertex];
            if (vertexGUI) {
                // the firstNode GUI is the current one, nodeGUI (the parent)
                secondNode = mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()][vertex->getNode()->getName()];
                vertexGUI->updateGUI(nodeGUI, secondNode);
                graphBoardScene_->update(); // what is the use of update

                /*
                vertexGUI->setLine(nodeGUI->x(), nodeGUI->y(), secondNode->x(), secondNode->y());
                vertexGUI->getWeightGUI()->setPos(secondNode->pos());
                */
            }
        }
        for (auto&& node : parentNodes) {
            for (auto&& vertex : graphViewer_->getCurrentGraph()->findVertices(node, nodeGUI->getNode())) {
                VertexGUI* verterxGUIParents = mapGraphsVerticesGUI_[graphViewer_->getCurrentGraph()->getName()][vertex];
                if (verterxGUIParents) {
                    NodeGUI* nodeGUIParent = mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()][node->getName()];
                    verterxGUIParents->updateGUI(nodeGUIParent, nodeGUI);
                    graphBoardScene_->update(); // what is the use of update...
                    /*
                    verterxGUIParents->setLine(nodeGUIParent->x(), nodeGUIParent->y(), nodeGUI->x(), nodeGUI->y());
                    verterxGUIParents->getWeightGUI()->setPos(nodeGUI->pos());
                    */
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

    void GraphViewerGUI::connectNodesCmd() {
        std::string firstNodeName = ui.firstNodeSelectedLineEdit->text().toStdString();
        std::string secondNodeName = ui.secondNodeSelectedLineEdit->text().toStdString();
        int weight = ui.vertexWeightSpinBox->value();
        if (firstNodeName != "" && secondNodeName != "") {
            std::shared_ptr<Model::Command> connectNodes;
            connectNodes = std::make_shared<Model::ConnectNodes>(graphViewer_->getCurrentGraph(), 
                mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()][firstNodeName]->getNode(), mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()][secondNodeName]->getNode(), weight);
            try {
                invoker_->executeCommand(connectNodes);
            }
            catch (Model::SameName& error) {}
        }
    }

    void GraphViewerGUI::connectNodesView(Model::Graph* graph, Model::vertex_sptr vertex) {
        // creating the vertex and setting up the position
        NodeGUI* firstNode = mapGraphsNodesGUI_[graph->getName()][vertex->getPreviousNode()->getName()];
        NodeGUI* secondNode = mapGraphsNodesGUI_[graph->getName()][vertex->getNode()->getName()];
        VertexGUI* vertexGUI = new VertexGUI(vertex, firstNode, secondNode);

        //creating the vertex weight GUI
        QGraphicsTextItem* vertexWeightGUI = new QGraphicsTextItem(QString::number(vertex->getWeight()), vertexGUI);
        QLineF vertexF(firstNode->x(), firstNode->y(), secondNode->x(), secondNode->y());
        QPointF weightPos = vertexF.pointAt(0.5);
        vertexWeightGUI->setPos(weightPos);
        vertexGUI->setWeightGUI(vertexWeightGUI);

        // I set the line in the constructor
        vertexGUI->setFlag(QGraphicsItem::ItemIsSelectable);

        // connect signals
        QObject::connect(vertexGUI, &VertexGUI::isSelected, this, &GraphViewerGUI::updateSelectedVertex);
        //graphBoardScene_->addItem(vertexWeightGUI);

        verifyCurrentGraph(graph);
        graphBoardScene_->addItem(vertexGUI);
        ui.vertexWeightSpinBox->setValue(1); // reinitialize the weight spinbox

        mapGraphsVerticesGUI_[graph->getName()][vertex] = vertexGUI;
        // set the currentselectednodegui (secondNode) to unselected so we can chain connection with the manageSelection
        secondNode->setSelected(false);

        //graphBoardScene_->update(); // when is update important?
    }

    void GraphViewerGUI::deleteVertexCmd() {
        if (selectedVertex_) {
            std::shared_ptr<Model::Command> deleteVertex;
            deleteVertex = std::make_shared<Model::DisconnectVertex>(graphViewer_->getCurrentGraph(),
                selectedVertex_->getVertex());
            invoker_->executeCommand(deleteVertex);
            selectedVertex_ = nullptr;
        }
    }

    void GraphViewerGUI::deleteVertexView(Model::Graph* graph, Model::vertex_sptr vertex) {
        ui.vertexSelectedLineEdit->setText("");

        VertexGUI* vertexToDelete = mapGraphsVerticesGUI_[graph->getName()][vertex];
        if (vertexToDelete) {
            verifyCurrentGraph(graph);
            graphBoardScene_->removeItem(vertexToDelete);
            mapGraphsVerticesGUI_[graph->getName()].erase(vertex);
            delete vertexToDelete;
            selectedVertex_ = nullptr;
        }
    }

    void GraphViewerGUI::updateSelectedVertex(VertexGUI* vertexGUI, bool isSelected) {
        if (isSelected) {
            selectedVertex_ = vertexGUI;
            std::string previousNodeName = vertexGUI->getVertex()->getPreviousNode()->getName();
            std::string nodeName = vertexGUI->getVertex()->getNode()->getName();
            ui.vertexSelectedLineEdit->setText(QString::fromStdString(previousNodeName + " -> " + nodeName));
            ui.vertexWeightSpinBox->setValue(vertexGUI->getVertex()->getWeight());
        }
        else {
            selectedVertex_ = nullptr;
            ui.vertexSelectedLineEdit->setText("");
            ui.vertexWeightSpinBox->setValue(0);
        }
    }

    void GraphViewerGUI::updateMinDistsCmd() {
        std::string rootName = ui.rootGraphLineEdit->text().toStdString();
        if (rootName == "") { return; }
        NodeGUI* rootGUI = mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()][rootName];
        if (rootGUI) {
            graphViewer_->getCurrentGraph()->updateMinDist(rootGUI->getNode());
        }
    }

    void GraphViewerGUI::updateMaxDistsCmd() {
        std::string rootName = ui.rootGraphLineEdit->text().toStdString();
        if (rootName == "") { return; }

        Model::node_sptr root = mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()][rootName]->getNode();
        if (root) {
            graphViewer_->getCurrentGraph()->updateMaxDist(root);
        }
    }

    void GraphViewerGUI::clearDistsItem() {
        if (!graphViewer_->getCurrentGraph()) { return; }
        for (auto&& pairNameAndnodeGUI : mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()]) {
            NodeGUI* nodeGUI = pairNameAndnodeGUI.second;
            if (nodeGUI->getDistsGUI()) {
                delete nodeGUI->getDistsGUI();
                nodeGUI->setDistsGUI(nullptr);
            }
        }
    }

    void GraphViewerGUI::updateMinDistsView(Model::Graph* graph) {
        verifyCurrentGraph(graph);
        clearDistsItem();

        for (auto&& pairNameAndnodeGUI : mapGraphsNodesGUI_[graph->getName()]) {
            NodeGUI* nodeGUI = pairNameAndnodeGUI.second;
            QGraphicsTextItem* minDistGUI;
            int minDist = nodeGUI->getNode()->getDistForMin();

            if (nodeGUI->getNode() == graphViewer_->getCurrentGraph()->getRoot()) {
                minDistGUI = new QGraphicsTextItem("min: 0");
            }
            else {
                if (minDist == INT_MAX) {
                    minDistGUI = new QGraphicsTextItem("min: none");
                }
                else {
                    minDistGUI = new QGraphicsTextItem("min: " + QString::number(minDist));
                }
            }

            nodeGUI->setDistsGUI(minDistGUI);
            minDistGUI->setPos(nodeGUI->x() + nodeGUI->getNodeNameDist(), nodeGUI->y() - nodeGUI->getNodeNameDist());
            graphBoardScene_->addItem(minDistGUI);
        }
    }

    void GraphViewerGUI::updateMaxDistsView(Model::Graph* graph) {
        verifyCurrentGraph(graph);
        clearDistsItem();

        for (auto&& pairNameAndnodeGUI : mapGraphsNodesGUI_[graph->getName()]) {
            NodeGUI* nodeGUI = pairNameAndnodeGUI.second;
            QGraphicsTextItem* maxDistGUI;
            int maxDist = nodeGUI->getNode()->getDistForMax();

            if (nodeGUI->getNode() == graphViewer_->getCurrentGraph()->getRoot()) {
                maxDistGUI = new QGraphicsTextItem("max: 0");
            }
            else {
                if (maxDist == INT_MIN) {
                    maxDistGUI = new QGraphicsTextItem("max: none");
                }
                else {
                    maxDistGUI = new QGraphicsTextItem("max: " + QString::number(maxDist));
                }
            }

            nodeGUI->setDistsGUI(maxDistGUI);
            maxDistGUI->setPos(nodeGUI->x() + nodeGUI->getNodeNameDist(), nodeGUI->y() - nodeGUI->getNodeNameDist());
            graphBoardScene_->addItem(maxDistGUI);
        }
    }

    void GraphViewerGUI::cancelCmd() {
        invoker_->cancel();
    }

    void GraphViewerGUI::redoCmd() {
        invoker_->redo();
    }
}