#pragma once
#include <qslider.h>
class ProgressSlider : public QSlider
{
public:
	ProgressSlider(QWidget *parent = NULL);
	~ProgressSlider();
protected:
	void paintEvent(QPaintEvent *paintEvent);
private:
	int bufferedAmount = 0;
};

