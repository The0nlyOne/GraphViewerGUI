#pragma once

#include "GraphViewer.hpp"
#include "NodeGUI.hpp"

#pragma warning(push, 0) // remove the useless warning of Qt?
#include <QtWidgets/QMainWindow>
#include <qgraphicsitem.h>
#include <qpainter.h>
#include <qgraphicssceneevent.h>
#pragma pop()

namespace View
{

	class VertexGUI : public QObject, public QGraphicsLineItem {//derive from QGraphicsObject instead of QGraphicsItem so that we can use signals and slots with it.
	Q_OBJECT
	public:
		/*
		VertexGUI() = default;
		VertexGUI(Model::vertex_sptr vertex) : vertex_(vertex)
		{
			setPen(QPen(QBrush(Qt::black), 2));
		}
		*/
		VertexGUI(Model::vertex_sptr vertex, NodeGUI* previousNodeGUI, NodeGUI* nodeGUI) : 
			vertex_(vertex), previousNodeGUI_(previousNodeGUI), nodeGUI_(nodeGUI)
		{
			qreal x1 = previousNodeGUI_->x(); qreal y1 = previousNodeGUI_->y();
			qreal x2 = nodeGUI_->x(); qreal y2 = nodeGUI_->y();

			QPen vertexPen = QPen(QBrush(Qt::black), 2);
			setPen(vertexPen);
			setLine(x1, y1, x2, y2);
			updateArrowAndWeightGUI(x1, y1, x2, y2);
		}

		~VertexGUI() {
			// nodeGUI pointers are deleted by the nodeGUI class
			delete firstLineForArrowGUI_;
			delete secondLineForArrowGUI_;
			delete weightGUI_;
		}

		void updateGUI(NodeGUI* nodeGUI, NodeGUI* secondNode) {
			qreal x1 = nodeGUI->x(); qreal y1 = nodeGUI->y();
			qreal x2 = secondNode->x(); qreal y2 = secondNode->y();

			setLine(x1, y1, x2, y2);
			updateArrowAndWeightGUI(x1, y1, x2, y2);
		}

		void updateArrowAndWeightGUI(qreal x1, qreal y1, qreal x2, qreal y2) {
			QLineF vertexF(x1, y1, x2, y2);

			if (weightGUI_) {
				QPointF weightPos = vertexF.pointAt(0.5);
				weightGUI_->setPos(weightPos);
			}

			qreal vertexAngle = vertexF.angle();
			QPointF arrowBegining = vertexF.pointAt(1); // inversed compared to the vertex direction
			QPointF arrowEnd = vertexF.pointAt(0.9); // so when we set the angle, it rotate correctly

			QLineF firstLineForArrow(arrowBegining, arrowEnd);
			firstLineForArrow.setAngle(vertexAngle + 155);
			QLineF secondLineForArrow(arrowBegining, arrowEnd);
			secondLineForArrow.setAngle(vertexAngle - 155);

			QGraphicsLineItem* firstLineForArrowGUI = new QGraphicsLineItem(firstLineForArrow, this);
			QGraphicsLineItem* secondLineForArrowGUI = new QGraphicsLineItem(secondLineForArrow, this);

			QPen vertexPen = QPen(QBrush(Qt::black), 2);
			firstLineForArrowGUI->setPen(vertexPen);
			secondLineForArrowGUI->setPen(vertexPen);

			if (firstLineForArrowGUI) { delete firstLineForArrowGUI_; }
			if (secondLineForArrowGUI_) { delete secondLineForArrowGUI_; }

			firstLineForArrowGUI_ = firstLineForArrowGUI;
			secondLineForArrowGUI_ = secondLineForArrowGUI;
		}

		Model::vertex_sptr getVertex() { return vertex_; }
		QGraphicsTextItem* getWeightGUI() { return weightGUI_; }
		void setWeightGUI(QGraphicsTextItem* weight) { weightGUI_ = weight; }

		QVariant itemChange(GraphicsItemChange change, const QVariant& value) {
			if (change == QGraphicsItem::ItemSelectedChange) {
				if (value.toBool()) { // is selected
					emit isSelected(this, true);
				}
				else { // isNotSelected check if amount of selected Node > 2
					emit isSelected(this, false);
				}
			}
			return QGraphicsItem::itemChange(change, value);
		}

	signals:
		void isSelected(VertexGUI* vertex, bool value);

	private:
		NodeGUI* previousNodeGUI_ = nullptr;
		NodeGUI* nodeGUI_ = nullptr;
		QGraphicsLineItem* firstLineForArrowGUI_ = nullptr; // a voir
		QGraphicsLineItem* secondLineForArrowGUI_ = nullptr;
		QGraphicsTextItem* weightGUI_ = nullptr;
		Model::vertex_sptr vertex_;
	};

}