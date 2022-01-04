#include "GraphViewerGUI.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    View::GraphViewerGUI w;
    w.initialize();
    w.show();
    return a.exec();
}
