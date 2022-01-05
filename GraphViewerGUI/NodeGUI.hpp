#pragma once

#include "GraphViewer.hpp"

#pragma warning(push, 0) // remove the useless warning of Qt?
#include <QtWidgets/QMainWindow>
#include <qgraphicsitem.h>
#include <qpainter.h>
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
		}
		QRectF boundingRect() const override
		{
			qreal penWidth = 1;
			return QRectF(-radius_ - penWidth / 2, -radius_ - penWidth / 2,
				diameter_ + penWidth, diameter_ + penWidth);
		}
		
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
		{
			painter->drawEllipse(0, 0, diameter_, diameter_);
		}

	private:
		int radius_ = 5;
		int diameter_ = radius_ * 2;
		std::string name_;
		int value_;
		int distMin_;
		int distMax_;
	};

}