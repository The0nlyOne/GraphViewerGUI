#pragma once

#include "GraphViewer.hpp"

#pragma warning(push, 0) // remove the useless warning of Qt?
#include <QtWidgets/QMainWindow>
#include <qgraphicsitem.h>
#include <qpainter.h>
#include <qgraphicssceneevent.h>
#pragma pop()

namespace View 
{

	class NodeGUI : public QObject, public QGraphicsItem { //we can derive from QGraphicsObject instead of QGraphicsItem so we can use signals and slots with it and remove QObject
		Q_OBJECT
	public:
		NodeGUI(Model::node_sptr node) :
			node_(node)
		{
			setAcceptDrops(true); // useless when ItemIsMovable
			setAcceptHoverEvents(true);
		}

		~NodeGUI() {
			delete nameGUI_;
			delete distsGUI_;
		}

		QRectF boundingRect() const override;
		
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

		Model::node_sptr getNode() { return node_; }
		
		QVariant itemChange(GraphicsItemChange change, const QVariant& value);

		void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

		void setNameGUI(QGraphicsTextItem* nameGUI) { nameGUI_ = nameGUI; }
		QGraphicsTextItem* getNameGUI() {  return nameGUI_; }

		void setDistsGUI(QGraphicsTextItem* distsGUI) { distsGUI_ = distsGUI; }
		QGraphicsTextItem* getDistsGUI() { return distsGUI_; }

		int getNodeNameDist() { return nodeNameDistance_; }
		void setNodeNameDist(int dist) { nodeNameDistance_ = dist; }

		void setBrush(QBrush brush) { brush_ = brush; }
		QBrush getBrush() { return brush_; }

	signals:
		void nodeReleased(NodeGUI* node, int x, int y);
		void nodeSelected(NodeGUI* node);

	private:
		Model::node_sptr node_ = nullptr;
		int radius_ = 9;
		int diameter_ = radius_ * 2;
		int nodeNameDistance_ = 20;
		QGraphicsTextItem* nameGUI_ = nullptr;
		QGraphicsTextItem* distsGUI_ = nullptr;
		QBrush brush_ = QBrush(Qt::darkCyan);
	};

}