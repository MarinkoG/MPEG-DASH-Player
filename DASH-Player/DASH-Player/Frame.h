#pragma once
#include <qlabel.h>
class Frame : public QLabel
{
	QPixmap frame;
public:
	Frame(QWidget *parent = NULL);
	~Frame();
	void showFrame(QPixmap *frame);

protected:
	void paintEvent(QPaintEvent *paintEvent);
};

