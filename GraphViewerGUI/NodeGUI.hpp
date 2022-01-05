#pragma once

#include "GraphViewer.hpp"
#include "GraphViewerGUI.hpp"

#pragma warning(push, 0) // remove the useless warning of Qt?
#include <QtWidgets/QMainWindow>
#include <qgraphicsitem.h>
#include <qpainter.h>
#include <qmimedata.h>
#include <qdrag.h>
#include <qgraphicssceneevent.h>
#pragma pop()

namespace View 
{
	class NodeGUI : public QGraphicsObject { //derive from QGraphicsObject instead of QGraphicsItem so that we can use signals and slots with it.
		Q_OBJECT
	public:
		NodeGUI() = default;
		NodeGUI(Model::node_sptr node) :
			name_(node->getName()), value_(node->getValue()), distMin_(node->getDistForMax()),
			distMax_(node->getDistForMax())
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
			QBrush blackBrush(Qt::black);
			painter->setBrush(blackBrush);
			painter->drawEllipse(0, 0, diameter_, diameter_);
			this->setOpacity(50);

		}

		void MousePressEvent(QGraphicsSceneMouseEvent* event) {
			QGraphicsItem::mousePressEvent(event);
			
			if (event->button() == Qt::MouseButton::LeftButton) {
				this->setSelected(!this->isSelected());
			}
			prepareGeometryChange();
			this->setOpacity(100);
		}

		void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
			QGraphicsItem::mouseReleaseEvent(event);
			emit nodeReleased(this->x(), this->y()); // to change 
		}
		/*
		void dragEnterEvent(QGraphicsSceneDragDropEvent* event)
		{
			event->setAccepted(event->mimeData()->hasFormat("text/plain"));
		}

		void mousePressEvent(QGraphicsSceneMouseEvent* event) {
			QMimeData* data = new QMimeData;
			QDrag* drag = new QDrag(event->widget());
			drag->setMimeData(data);
			drag->exec();
		}
		*/

	signals:
		void nodeReleased(int x, int y);

	private:
		int radius_ = 5;
		int diameter_ = radius_ * 2;
		std::string name_;
		int value_;
		int distMin_;
		int distMax_;
	};

}