#include "NodeGUI.hpp"

namespace View
{
	QRectF NodeGUI::boundingRect() const
	{
		qreal penWidth = 1;
		return QRectF(0 + penWidth, 0 + penWidth, diameter_ + penWidth, diameter_ + penWidth);
	}

	void NodeGUI::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
	{
		painter->setBrush(brush_);
		painter->drawEllipse(0, 0, diameter_, diameter_);
	}

	QVariant NodeGUI::itemChange(GraphicsItemChange change, const QVariant& value) { // Cant add it in cpp file for some reason
		if (change == QGraphicsItem::ItemSelectedChange) {
			if (value.toBool()) { // is selected
				brush_ = QBrush(Qt::yellow);
				update();
				emit nodeSelected(this);
			}
			else { // isNotSelected check if amount of selected Node > 2
				if (brush_.color() != Qt::red) {
					brush_ = QBrush(Qt::darkCyan);
					update();
				}
			}
		}
		return QGraphicsItem::itemChange(change, value);
	}

	void NodeGUI::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
		QGraphicsItem::mouseReleaseEvent(event);
		emit nodeReleased(this, this->x(), this->y());
	}

}