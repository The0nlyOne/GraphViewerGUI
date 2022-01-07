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
		VertexGUI(Model::vertex_sptr vertex, NodeGUI* previousNodeGUI, NodeGUI* nodeGUI);

		~VertexGUI();

		void updateGUI(NodeGUI* nodeGUI, NodeGUI* secondNode);

		void updateArrowAndWeightGUI(qreal x1, qreal y1, qreal x2, qreal y2);

		Model::vertex_sptr getVertex() { return vertex_; }
		QGraphicsTextItem* getWeightGUI() { return weightGUI_; }
		void setWeightGUI(QGraphicsTextItem* weight) { weightGUI_ = weight; }

		QVariant itemChange(GraphicsItemChange change, const QVariant& value);

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