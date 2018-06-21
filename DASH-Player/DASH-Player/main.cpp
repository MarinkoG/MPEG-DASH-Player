#include "DASHPlayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DASHPlayer w;
	w.show();
	return a.exec();
}
