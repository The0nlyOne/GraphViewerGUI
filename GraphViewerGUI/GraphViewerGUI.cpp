#include "GraphViewerGUI.hpp"

namespace View
{
    GraphViewerGUI::GraphViewerGUI(QWidget* parent)
        : QMainWindow(parent)
    {
        ui.setupUi(this);
    }

    void GraphViewerGUI::initialize()
    {
        QScrollArea* graphBoard = ui.graphScrollArea;

        QCheckBox* checkBox1 = new QCheckBox("A");

        checkBox1->setParent(graphBoard);
        // checkBox1->move(0, 0);
        QCheckBox* checkBox2 = new QCheckBox("B");
        if (ui.graphScrollArea->childAt(0, 0)) {
            checkBox2->move(40, 0);
            checkBox2->setParent(graphBoard);
        }

    }

}