#pragma once
#include <qlabel.h>
#include <qpainter.h>

class Frame : public QLabel
{

public:
	Frame(QWidget *parent = NULL);
	~Frame();
	void showFrame(QPixmap *frame);

protected:
	void paintEvent(QPaintEvent *paintEvent);

private:
	QPixmap frame;
};
