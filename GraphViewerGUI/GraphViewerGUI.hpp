#pragma once

#include "Invoker.hpp"
#include "GraphViewer.hpp"
#include "Graph.hpp"

#pragma warning(push, 0) // remove the useless warning of Qt?
#include <QtWidgets/QMainWindow>
#include <qcheckbox.h>
#include "ui_GraphViewerGUI.h"
#pragma pop()

namespace View
{
    class GraphViewerGUI : public QMainWindow
    {
        Q_OBJECT

    public:
        GraphViewerGUI(QWidget* parent = Q_NULLPTR);
        void initialize();

    private:
        Ui::GraphViewerGUIClass ui;
    };
}
