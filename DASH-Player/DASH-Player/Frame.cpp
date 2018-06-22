#include "Frame.h"

Frame::Frame(QWidget *parent) :QLabel(parent)
{
}

Frame::~Frame()
{
}

void Frame::showFrame(QPixmap *frame)
{
	this->frame = *frame;
	update();
}

void Frame::paintEvent(QPaintEvent * paintEvent)
{
	QPainter painter;
	painter.begin(this);
	painter.drawPixmap(this->rect(), this->frame);
	painter.end();
}
