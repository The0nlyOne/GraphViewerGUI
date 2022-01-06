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
		VertexGUI() = default;
		VertexGUI(Model::vertex_sptr vertex) : vertex_(vertex)
		{
			setPen(QPen(QBrush(Qt::black), 2));
		}

		VertexGUI(Model::vertex_sptr vertex, NodeGUI* previousNodeGUI, NodeGUI* nodeGUI) : 
			vertex_(vertex), previousNodeGUI_(previousNodeGUI), nodeGUI_(nodeGUI)
		{
			setLine(previousNodeGUI->x(), previousNodeGUI->y(), nodeGUI->x(), nodeGUI->y());
			setPen(QPen(QBrush(Qt::black), 2));
		}

		Model::vertex_sptr getVertex() { return vertex_; }
		QGraphicsTextItem* getWeightGUI() { return weightGUI_; }
		void setWeightGUI(QGraphicsTextItem* weight) { weightGUI_ = weight; }

		QVariant itemChange(GraphicsItemChange change, const QVariant& value) { // it is called too many time for what I am using it
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
		QGraphicsTextItem* weightGUI_ = nullptr;
		Model::vertex_sptr vertex_;
	};

}