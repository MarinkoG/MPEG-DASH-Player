#include "MediaEngine.h"

MediaEngine::MediaEngine(Frame *video) : video(video)
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
	segments = segmentFactory->createSegments(0, currentSegmentNumber);
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
	segmentDecoder = new SegmentDecoder(&frameBuffer, &frameBufferMutex, &frameBufferNotEmpty, &segmentBuffer, &segmentBufferMutex, &segmentBufferNotEmpty);
	QObject::connect(segmentDecoder, &SegmentDecoder::segmentDecoded, this, &MediaEngine::saveNumberOfFrames);
	segmentDecoder->start();
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

void MediaEngine::saveNumberOfFrames(long numberOfFrames)
{
	segmentFrameNumbers.push_back(numberOfFrames);
}
