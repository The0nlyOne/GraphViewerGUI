#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GraphViewerGUI.h"

class GraphViewerGUI : public QMainWindow
{
    Q_OBJECT

public:
    GraphViewerGUI(QWidget *parent = Q_NULLPTR);

private:
    Ui::GraphViewerGUIClass ui;
};
