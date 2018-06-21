#include "ProgressSlider.h"
#include <qpainter.h>

ProgressSlider::ProgressSlider(QWidget *parent) :QSlider(parent)
{
}


ProgressSlider::~ProgressSlider()
{
}

void ProgressSlider::paintEvent(QPaintEvent * paintEvent)
{
	QPainter painter;
	painter.begin(this);
	painter.setBrush(Qt::gray);
	painter.drawRect(0, 5, bufferedAmount, this->rect().height() - 11);
	painter.end();
	QSlider::paintEvent(paintEvent);
}