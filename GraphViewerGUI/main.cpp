#include "GraphViewerGUI.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphViewerGUI w;
    w.show();
    return a.exec();
}
