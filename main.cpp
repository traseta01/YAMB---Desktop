#include "glavniprozor.h"
#include <QApplication>
#include <ctime>

int main(int argc, char *argv[])
{
	std::srand(time(NULL));
    QApplication a(argc, argv);
    GlavniProzor w;
	w.setWindowTitle("YAMB");
    w.show();

    return a.exec();
}
