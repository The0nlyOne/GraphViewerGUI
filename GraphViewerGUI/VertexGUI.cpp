#include "VertexGUI.hpp"

namespace View {

	VertexGUI::VertexGUI(Model::vertex_sptr vertex, NodeGUI* previousNodeGUI, NodeGUI* nodeGUI) :
		vertex_(vertex), previousNodeGUI_(previousNodeGUI), nodeGUI_(nodeGUI)
	{
		qreal x1 = previousNodeGUI_->x(); qreal y1 = previousNodeGUI_->y();
		qreal x2 = nodeGUI_->x(); qreal y2 = nodeGUI_->y();

		QPen vertexPen = QPen(QBrush(Qt::black), 2);
		setPen(vertexPen);
		setLine(x1, y1, x2, y2);
		updateArrowAndWeightGUI(x1, y1, x2, y2);
	}

	VertexGUI::~VertexGUI() {
		// nodeGUI pointers are deleted by the nodeGUI class
		delete firstLineForArrowGUI_;
		delete secondLineForArrowGUI_;
		delete weightGUI_;
	}

	void VertexGUI::updateGUI(NodeGUI* nodeGUI, NodeGUI* secondNode) {
		qreal x1 = nodeGUI->x(); qreal y1 = nodeGUI->y();
		qreal x2 = secondNode->x(); qreal y2 = secondNode->y();

		setLine(x1, y1, x2, y2);
		updateArrowAndWeightGUI(x1, y1, x2, y2);
	}

	// could set a max and min for arrom lenght
	void VertexGUI::updateArrowAndWeightGUI(qreal x1, qreal y1, qreal x2, qreal y2) {
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

	QVariant VertexGUI::itemChange(GraphicsItemChange change, const QVariant& value) {
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
}

