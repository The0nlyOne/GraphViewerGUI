#include "GraphViewerGUI.hpp"
#include "FileManager.hpp"

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
        QObject::connect(ui.saveFileButton, SIGNAL(clicked()), this, SLOT(saveFileCmd()));
        QObject::connect(ui.openFileButton, SIGNAL(clicked()), this, SLOT(openFileCmd()));
        QObject::connect(ui.zoomHSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomSlideView(int)));
        QObject::connect(ui.zoomPushButton, SIGNAL(clicked()), this, SLOT(zoomView()));
        QObject::connect(ui.dezoomPushButton, SIGNAL(clicked()), this, SLOT(dezoomView()));


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
        // protection
        if (!graph) { return; }

        QObject::connect(graph, &Model::Graph::nodeAddedSignal, this, &GraphViewerGUI::addNodeView);
        QObject::connect(graph, &Model::Graph::nodeDeletedSignal, this, &GraphViewerGUI::deleteNodeView);
        QObject::connect(graph, &Model::Graph::vertexAddedSignal, this, &GraphViewerGUI::connectNodesView);
        QObject::connect(graph, &Model::Graph::vertexDeletedSignal, this, &GraphViewerGUI::deleteVertexView);
        QObject::connect(graph, &Model::Graph::minDistUpdatedSignal, this, &GraphViewerGUI::updateMinDistsView);
        QObject::connect(graph, &Model::Graph::maxDistUpdatedSignal, this, &GraphViewerGUI::updateMaxDistsView);
    }

    void GraphViewerGUI::addGraphCmd()
    {
        if (ui.graphNamelineEdit->text() == "") { return; }

        std::string name = ui.graphNamelineEdit->text().toStdString();

        if (graphsMap_[name]) { return; } // does not try to add an already existing graph

        Model::graph_sptr graphToAdd = std::make_shared<Model::Graph>(name);
        std::shared_ptr<Model::Command> addGraph = std::make_shared<Model::AddGraph>(graphViewer_, graphToAdd);

        connectGraph(graphToAdd.get());

        invoker_->executeCommand(addGraph);
    }

    void GraphViewerGUI::addGraphView(Model::graph_sptr graph) {
        // protection
        if (!graph) { return; }

        connectGraph(graph.get()); // in case we load the graph with a file and did not go through the cmd

        std::string graphName = graph->getName();
        ui.graphsListWidget->addItem(QString::fromStdString(graphName));
        graphsMap_[graphName] = graph;

        changeCurrentGraphView(graphName);

        ui.graphNamelineEdit->setText("");
    }

    void GraphViewerGUI::changeGraphCmd(QListWidgetItem* current, QListWidgetItem* previous) {
        if (current) {
            std::string graphName = current->text().toStdString();
            graphViewer_->changeGraph(graphsMap_[graphName]); // will emit the graphChanged signal
        }
        else {
            clearGUI();
            graphViewer_->changeGraph(nullptr);
        }
    }

    void GraphViewerGUI::changeGraphView(Model::graph_sptr graph) {
        clearGUI();

        if (!graph) { return; }

        std::string graphName = graph->getName();
        mapGraphsNodesGUI_[graphName].clear();
        mapGraphsVerticesGUI_[graphName].clear();


        for (auto&& node : graph->getNodes()) {
            addNodeView(graph.get(), node);
        }

        for (auto&& vertex : graph->getVertices()) {
            connectNodesView(graph.get(), vertex);
        }
    }

    void GraphViewerGUI::clearGraphCmd() {
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
        if (!graph) { return; }

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
        // protections
        if (!graph || !node) { return; }

        // verify that the node is not already in the view, in case it adds twice...
        if (mapGraphsNodesGUI_[graph->getName()][node->getName()]) { return; }

        verifyCurrentGraph(graph);

        // creating the node and setting its position
        NodeGUI* nodeGUI = new NodeGUI(node);
        int nodeX = node->getPos().first;
        int nodeY = node->getPos().second;

        if (nodeX == 0 && nodeY == 0) { // it means the node has just been created.
            int newX = newNodePos_.x();
            int newY = newNodePos_.y();
            nodeGUI->setX(newX);
            nodeGUI->setY(newY);
            nodeGUI->getNode()->setPos(newX, newY);
        }
        else {
            nodeGUI->setX(nodeX);
            nodeGUI->setY(nodeY);
            newNodePos_ = QPoint(nodeX, nodeY);
        }
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
        // protections
        if (!graph || !node) { return; }

        verifyCurrentGraph(graph);

        NodeGUI* nodeToDelete = mapGraphsNodesGUI_[graph->getName()][node->getName()];

        // protection in case the method is called twice in a row. Still don't know why.
        if (!nodeToDelete) { return; }

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
        case 0: // no nodes selected
            selectedNodeCount_++;
            ui.firstNodeSelectedLineEdit->setText(QString::fromStdString(nodeGUI->getNode()->getName()));
            previousFirstConnectedNode_ = nodeGUI;
            break;
        case 1: // 1 node is selected
            selectedNodeCount_++;
            ui.secondNodeSelectedLineEdit->setText(QString::fromStdString(nodeGUI->getNode()->getName()));
            previousSecondConnectedNode_ = nodeGUI;
            break;
        case 2: // 2 nodes is selected
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
    }

    void GraphViewerGUI::setNewNodePos(NodeGUI* nodeGUI, int x, int y) {
        // protection
        if (!nodeGUI) { return; }

        newNodePos_.setX(x); newNodePos_.setY(y);
        nodeGUI->getNode()->setPos(x, y);
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
            }
        }
        for (auto&& node : parentNodes) {
            for (auto&& vertex : graphViewer_->getCurrentGraph()->findVertices(node, nodeGUI->getNode())) {
                VertexGUI* verterxGUIParents = mapGraphsVerticesGUI_[graphViewer_->getCurrentGraph()->getName()][vertex];
                if (verterxGUIParents) {
                    NodeGUI* nodeGUIParent = mapGraphsNodesGUI_[graphViewer_->getCurrentGraph()->getName()][node->getName()];
                    verterxGUIParents->updateGUI(nodeGUIParent, nodeGUI);
                }
            }
        }
    }

    void GraphViewerGUI::verifyNodePos() {
        bool goodPos = false;
        int countOfLoop = 0;

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
            if (countOfLoop == 100) {
                return; // avoid infinite loop because the program could not find a good position.
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
        if (!graph || !vertex) { return; }

        verifyCurrentGraph(graph);

        // verify that the vertex is not already in the view, in case it adds twice...
        if (mapGraphsVerticesGUI_[graph->getName()][vertex]) { return; }

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

        verifyCurrentGraph(graph);
        graphBoardScene_->addItem(vertexGUI);
        ui.vertexWeightSpinBox->setValue(1); // reinitialize the weight spinbox

        mapGraphsVerticesGUI_[graph->getName()][vertex] = vertexGUI;
        // set the currentselectednodegui (secondNode) to unselected so we can chain connection with the manageSelection
        secondNode->setSelected(false);
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
        if (!graph || !vertex) { return; }

        ui.vertexSelectedLineEdit->setText("");

        VertexGUI* vertexToDelete = mapGraphsVerticesGUI_[graph->getName()][vertex];
        if (!vertexToDelete) { return; }

        verifyCurrentGraph(graph);
        graphBoardScene_->removeItem(vertexToDelete);
        mapGraphsVerticesGUI_[graph->getName()].erase(vertex);
        delete vertexToDelete;
        selectedVertex_ = nullptr;
    }

    void GraphViewerGUI::updateSelectedVertex(VertexGUI* vertexGUI, bool isSelected) {
        if (!vertexGUI) { return; }

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
            if (nodeGUI && nodeGUI->getDistsGUI()) {
                delete nodeGUI->getDistsGUI();
                nodeGUI->setDistsGUI(nullptr);
            }
        }
    }

    void GraphViewerGUI::updateMinDistsView(Model::Graph* graph) {
        if (!graph) { return; }

        verifyCurrentGraph(graph);
        clearDistsItem();

        for (auto&& pairNameAndnodeGUI : mapGraphsNodesGUI_[graph->getName()]) {
            if (!pairNameAndnodeGUI.second) { continue; }
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
        if (!graph) { return; }

        verifyCurrentGraph(graph);
        clearDistsItem();

        for (auto&& pairNameAndnodeGUI : mapGraphsNodesGUI_[graph->getName()]) {
            if (!pairNameAndnodeGUI.second) { continue; }
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

    void GraphViewerGUI::saveFileCmd() {
        QString filename = QFileDialog::getSaveFileName(
            this, tr("Open FIle"), "./", tr("Graph Viewer Files (*.grv)"));

        fileManager_->saveFile(filename.toStdString());
    }

    void GraphViewerGUI::openFileCmd() {
        QString filename = QFileDialog::getOpenFileName(
            this, tr("Open FIle"), "./", tr("Graph Viewer Files (*.grv)"));
        fileManager_->loadFile(filename.toStdString());
    }

    void GraphViewerGUI::zoomSlideView(int value) {
        previousZoomValue_ < value ? graphBoardView_->scale(1.05, 1.05) : graphBoardView_->scale(1 / 1.05, 1 / 1.05);
        previousZoomValue_ = value;
    }

    void GraphViewerGUI::zoomView() {
        graphBoardView_->scale(1.2, 1.2);
    }

    void GraphViewerGUI::dezoomView() {
        graphBoardView_->scale(1 / 1.2, 1 / 1.2);
    }

}