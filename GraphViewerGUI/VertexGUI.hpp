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

		void mousePressEvent(QGraphicsSceneMouseEvent* event) {
			emit vertexPressed(this);
		}

	signals:
		void vertexPressed(VertexGUI* vertex);

	private:
		NodeGUI* previousNodeGUI_;
		NodeGUI* nodeGUI_;
		Model::vertex_sptr vertex_;
	};

}