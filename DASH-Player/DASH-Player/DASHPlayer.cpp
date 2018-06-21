#include "DASHPlayer.h"
#include <string.h>
#include <iostream>
using namespace std;

DASHPlayer::DASHPlayer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->video = ui.video;
}

void DASHPlayer::on_downloadButton_clicked() 
{
	QString mpdUrl = ui.mpdEdit->text();
	ui.statusBar->showMessage(mpdUrl);
}

void DASHPlayer::on_playButton_clicked() 
{
}

void DASHPlayer::on_progressSlider_valueChanged()
{
}

void DASHPlayer::on_videoQuality_currentIndexChanged()
{
}
