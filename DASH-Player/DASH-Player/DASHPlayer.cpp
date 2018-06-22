#include "DASHPlayer.h"

DASHPlayer::DASHPlayer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void DASHPlayer::on_downloadButton_clicked()
{
	QString *response;
	mediaEngine = new MediaEngine(ui.video);
	QString mpdUrl = ui.mpdEdit->text();
	url = mpdUrl.toStdString();
	regex urlRegex("(?:ftp|http|https)+://([\\S\^<\^>]+)");

	if (regex_match(url, urlRegex)) //check url validity
	{
		if (mediaEngine->parseMPD(url)) //if mpd is valid download and then parse it
		{
			response = new QString("Successfully parsed MPD!");
			ui.playButton->setEnabled(true);
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
	myfile.open("Izlaz.txt", ios::app);
	myfile << string << endl;
	myfile.close();
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
