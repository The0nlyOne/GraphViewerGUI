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
	class NodeGUI : public QGraphicsObject { //derive from QGraphicsObject instead of QGraphicsItem so that we can use signals and slots with it.
		Q_OBJECT
	public:
		NodeGUI() = default;
		NodeGUI(Model::node_sptr node) :
			node_(node)
		{
			setAcceptDrops(true); // useless when ItemIsMovable
			setAcceptHoverEvents(true);
		}
		QRectF boundingRect() const override
		{
			qreal penWidth = 1;
			/*
			return QRectF(-radius_ - penWidth / 2, -radius_ - penWidth / 2,
				diameter_ + penWidth, diameter_ + penWidth);
			*/
			return QRectF(0 + penWidth, 0 + penWidth, diameter_ + penWidth, diameter_ + penWidth);
		}
		
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
		{
			painter->setBrush(brush_);
			painter->drawEllipse(0, 0, diameter_, diameter_);
		}

		Model::node_sptr getNode() { return node_; }
		
		QVariant itemChange(GraphicsItemChange change, const QVariant& value) { // it is called too many time for what I am using it
			if (change == QGraphicsItem::ItemSelectedChange) {
				if (value.toBool()) { // is selected
					brush_ = QBrush(Qt::yellow);
					update();
					emit nodeSelected(this);
				}
				else { // isNotSelected check if amount of selected Node > 2
					if (brush_.color() != Qt::red) {
						brush_ = QBrush(Qt::black);
						update();
					}
				}
			}
			return QGraphicsItem::itemChange(change, value);
		}

		void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
			QGraphicsItem::mouseReleaseEvent(event);
			emit nodeReleased(this, this->x(), this->y()); // to change 
		}

		void setBrush(QBrush brush) { brush_ = brush; }
		QBrush getBrush() { return brush_; }

	signals:
		void nodeReleased(NodeGUI* node, int x, int y);
		void nodeSelected(NodeGUI* node);

	private:
		Model::node_sptr node_;
		int radius_ = 7;
		int diameter_ = radius_ * 2;
		QBrush brush_ = QBrush(Qt::black);
	};

}