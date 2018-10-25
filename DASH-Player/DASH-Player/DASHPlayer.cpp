#include "DASHPlayer.h"

DASHPlayer::DASHPlayer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	video = ui.video;
	width = this->width;
	height = this->height;
	mpdParser = new MPDParser();
}

void DASHPlayer::on_downloadButton_clicked()
{
	QString *response;
	QString mpdUrl = ui.mpdEdit->text();
	url = mpdUrl.toStdString();
	regex urlRegex("(?:ftp|http|https)+://([\\S\^<\^>]+)");

	if (regex_match(url, urlRegex)) //check url validity
	{
		if (mpdParser->parseMPD(url)) //if mpd is valid download and then parse it
		{
			response = new QString("Successfully parsed MPD!");
			setUI();
		}
		else
		{
			response = new QString("ERROR: Could not parse mpd!");
		}
	}
	else
	{
		response = new QString("ERROR: Not valid uri!");
	}
	ui.statusBar->showMessage(*response);
}

void DASHPlayer::print(string string)  //for debuging and testing purposes
{
	ofstream myfile;
	myfile.open("Log.txt", ios::app);
	myfile << string << endl;
	myfile.close();
}

void DASHPlayer::setUI()
{
	ui.videoQuality->addItems(mpdParser->getVideoQualities());
	ui.playButton->setEnabled(true);
}

void DASHPlayer::on_playButton_clicked()
{
	mediaEngine = new MediaEngine(mpdParser->getMPD(), video);
	mediaEngine->setVideoQuality(ui.videoQuality->currentText());
	mediaEngine->start();
}

void DASHPlayer::on_progressSlider_valueChanged()
{
}

void DASHPlayer::on_videoQuality_currentIndexChanged()
{
}
