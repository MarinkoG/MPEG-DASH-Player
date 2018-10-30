#include "MediaEngine.h"

MediaEngine::MediaEngine(IMPD *mpd, Frame *video) :
	mpd(mpd),
	video(video)
{
	currentSegmentNumber = -1;
}

MediaEngine::~MediaEngine()
{
}

bool MediaEngine::parseMPD(string uri)
{
	if (dash::IDASHManager *dashManager = CreateDashManager())
	{
		if (mpd = dashManager->Open((char*)uri.c_str()))
		{
			return true;
		}
	}
	return false;
}

IMPD * MediaEngine::getMPD()
{
	return this->mpd;
}

bool MediaEngine::start()
{
	showLoadingScreen();
	createSegments(currentSegmentNumber);
	downloadSegments();
	return false;
}

bool MediaEngine::createSegments(long currentSegmentNumber)
{

	/*
	segmentFactory = new SegmentFactory(mpd);
	videoSegments = segmentFactory->createSegments(bandwidth, currentSegmentNumber);
	return true;

	*/


	segmentFactory = new SegmentFactory(mpd);
	audioSegments = segmentFactory->createAudioSegments();
	return true;
}

void MediaEngine::downloadSegments()
{
	/*
	segmentDownloader = new SegmentDownloader(&videoSegments, &videoSegmentBuffer,&videoSegmentBufferMutex,&videoSegmentBufferNotEmpty);
	QObject::connect(segmentDownloader, &SegmentDownloader::segmentDownloaded, this, &MediaEngine::startDecoding);
	segmentDownloader->start();
	*/



	segmentDownloader = new SegmentDownloader(&audioSegments, &audioSegmentBuffer, &audioSegmentBufferMutex, &audioSegmentBufferNotEmpty);
	QObject::connect(segmentDownloader, &SegmentDownloader::segmentDownloaded, this, &MediaEngine::startDecoding);
	segmentDownloader->start();

}

void MediaEngine::decodeVideoSegments()
{
	videoDecoder = new VideoDecoder(&videoSegmentBuffer, &videoSegmentBufferMutex, &videoSegmentBufferNotEmpty, &frameBuffer, &frameBufferMutex, &frameBufferNotEmpty, &frameBufferNotFull);
	QObject::connect(videoDecoder, &VideoDecoder::segmentDecoded, this, &MediaEngine::saveNumberOfFrames);
	QObject::connect(videoDecoder, &VideoDecoder::framesDecoded, this, &MediaEngine::startRendering);
	videoDecoder->start();
}

void MediaEngine::renderVideo(double framerate)
{
	adjustPlayerSize();
	videoRenderer = new VideoRenderer(video, &frameBuffer, &frameBufferMutex, &frameBufferNotEmpty, &frameBufferNotFull);
	videoRenderer->setFramerate(framerate);
	videoRenderer->start();
}

void MediaEngine::print(string string) // for testing
{
	ofstream myfile;
	myfile.open("Log.txt", ios::app);
	myfile << string << endl;
	myfile.close();
}

void MediaEngine::startDecoding()
{
	//saveSegment();

	
	if (!decodingStarted)
	{
		//decodeVideoSegments();
		decodeAudioSegments();
		decodingStarted = true;
	}
	

}

void MediaEngine::startRendering(double framerate)
{
	print("broj  samplova po kanalu u framu" + to_string(framerate));
	/**
	if (!renderingStarted)
	{
		renderVideo(framerate);
		renderingStarted = true;
	}
	*/
}

void MediaEngine::startAudioRendering(QAudioFormat *format)
{
	audioRenderer = new AudioRenderer(&audioSampleBuffer, &audioSampleBufferMutex, &audioSampleBufferNotEmpty, &audioSampleBufferNotFull);
	audioRenderer->setAudioFormat(format);
	audioRenderer->start();

}

void MediaEngine::saveNumberOfFrames(long numberOfFrames)
{
	print("broj samplova " + to_string(numberOfFrames));
	segmentFrameNumbers.push_back(numberOfFrames);
}

void MediaEngine::adjustPlayerSize()
{
	video->resize(width, height);
	video->setMinimumSize(width, height);
	QWidget *player = video->parentWidget();

	while (player->parentWidget())
	{
		player = player->parentWidget();
	}
	player->adjustSize();
}

void MediaEngine::setRepresentation(IRepresentation * representation)
{
	this->representation = representation;
}

void MediaEngine::setBandwidth(int bandwidth)
{
	this->bandwidth = bandwidth;
}

void MediaEngine::setVideoQuality(QString videoQuality)
{
	videoQuality.chop(1);
	width = videoQuality.split("x").at(0).toInt();
	videoQuality = videoQuality.split("x").at(1);
	height = videoQuality.split(" (").at(0).toInt();
	bandwidth = videoQuality.split(" (").at(1).toInt();
}

void MediaEngine::showLoadingScreen()
{
	QImage *image = NULL;
	if (image == NULL)
	{
		image = new QImage(video->width(), video->height(), QImage::Format_RGB32);
	}
	QPainter p(image);
	p.setPen(QPen(Qt::white));
	p.setFont(QFont("Times", 20, QFont::Bold));
	p.drawText(image->rect(), Qt::AlignCenter, "LOADING");
	video->showFrame(&(QPixmap::fromImage(*image)));
}

void MediaEngine::decodeAudioSegments()
{
	audioDecoder = new AudioDecoder(&audioSegmentBuffer, &audioSegmentBufferMutex, &audioSegmentBufferNotEmpty, &audioSampleBuffer, &audioSampleBufferMutex, &audioSampleBufferNotEmpty, &audioSampleBufferNotFull);
	QObject::connect(audioDecoder, &AudioDecoder::segmentDecoded, this, &MediaEngine::saveNumberOfFrames);
	QObject::connect(audioDecoder, &AudioDecoder::framesDecoded, this, &MediaEngine::startAudioRendering);
	audioDecoder->start();
}

void MediaEngine::saveSegment() // for testing
{
	int i = 1;
	for each (ISegment* segment in audioSegmentBuffer)
	{
		ofstream file;
		string extension = ".mpa";
		string fileName = "audioo" + to_string(i);
		fileName = fileName + extension;
		file.open(fileName, ios::out | ios::binary);
		size_t len = 32768;
		uint8_t *p_data = new uint8_t[32768];
		int ret = 0;
		do
		{
			ret = segment->Read(p_data, len);
			if (ret > 0)
			{
				(&file)->write((char *)p_data, ret);
			}
		} while (ret > 0);
		file.close();
		i++;
	}
}