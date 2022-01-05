#pragma once

#include "GraphViewer.hpp"

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
	class NodeGUI : public QGraphicsItem {
	public:
		NodeGUI() = default;
		NodeGUI(Model::node_sptr node) :
			name_(node->getName()), value_(node->getValue()), distMin_(node->getDistForMax()),
			distMax_(node->getDistForMax())
		{
			setAcceptDrops(true);
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
			//QBrush blackBrush(Qt::black);
			//painter->setBrush(blackBrush);
			painter->drawEllipse(0, 0, diameter_, diameter_);

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
	private:
		int radius_ = 5;
		int diameter_ = radius_ * 2;
		std::string name_;
		int value_;
		int distMin_;
		int distMax_;
	};

}