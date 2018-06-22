#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_DASHPlayer.h"
#include "Frame.h"
#include "MediaEngine.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <regex>

using namespace std;
using namespace dash::mpd;

class DASHPlayer : public QMainWindow
{
	Q_OBJECT

public:
	DASHPlayer(QWidget *parent = Q_NULLPTR);
	void print(string string);

private:
	Ui::DASHPlayerClass ui;
	Frame *video;
	MediaEngine *mediaEngine;
	string url;

private slots:
	void on_downloadButton_clicked();
	void on_playButton_clicked();
	void on_progressSlider_valueChanged();
	void on_videoQuality_currentIndexChanged();
};
