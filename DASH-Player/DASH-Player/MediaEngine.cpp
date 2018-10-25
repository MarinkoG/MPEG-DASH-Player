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
	createSegments(currentSegmentNumber);
	downloadSegments();
	return false;
}

bool MediaEngine::createSegments(long currentSegmentNumber)
{
	segmentFactory = new SegmentFactory(mpd);
	segments = segmentFactory->createSegments(bandwidth, currentSegmentNumber);
	return true;
}

void MediaEngine::downloadSegments()
{
	segmentDownloader = new SegmentDownloader(&segments, &segmentBuffer,&segmentBufferMutex,&segmentBufferNotEmpty);
	QObject::connect(segmentDownloader, &SegmentDownloader::segmentDownloaded, this, &MediaEngine::startDecoding);
	segmentDownloader->start();
}

void MediaEngine::decodeSegments()
{
	segmentDecoder = new SegmentDecoder(&frameBuffer, &frameBufferMutex, &frameBufferNotEmpty, &frameBufferNotFull, &segmentBuffer, &segmentBufferMutex, &segmentBufferNotEmpty);
	QObject::connect(segmentDecoder, &SegmentDecoder::segmentDecoded, this, &MediaEngine::saveNumberOfFrames);
	QObject::connect(segmentDecoder, &SegmentDecoder::framesDecoded, this, &MediaEngine::startRendering);
	segmentDecoder->start();
}

void MediaEngine::renderVideo()
{
	adjustPlayerSize();
	videoRenderer = new VideoRenderer(video, &frameBuffer, &frameBufferMutex, &frameBufferNotEmpty, &frameBufferNotFull);
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
	if (!decodingStarted)
	{
		decodeSegments();
		decodingStarted = true;
	}
}

void MediaEngine::startRendering()
{
	if (!renderingStarted)
	{
		renderVideo();
		renderingStarted = true;
	}
}

void MediaEngine::saveNumberOfFrames(long numberOfFrames)
{
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
