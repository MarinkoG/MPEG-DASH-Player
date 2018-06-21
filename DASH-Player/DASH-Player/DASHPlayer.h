#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DASHPlayer.h"
#include "Frame.h"

class DASHPlayer : public QMainWindow
{
	Q_OBJECT

public:
	DASHPlayer(QWidget *parent = Q_NULLPTR);
private:
	Ui::DASHPlayerClass ui;
	Frame *video;
private slots:
	void on_downloadButton_clicked();
	void on_playButton_clicked();
	void on_progressSlider_valueChanged();
	void on_videoQuality_currentIndexChanged();
};