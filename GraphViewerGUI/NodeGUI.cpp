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


}