#pragma once

#include "Frame.h"
#include <IMPD.h>
#include <string.h>
#include <IDASHManager.h>
#include <qobject.h>
#include "libdash.h"
#include <deque>
#include "SegmentFactory.h"
#include "SegmentDownloader.h"
#include "SegmentDecoder.h"
#include <fstream>
#include <iostream>
#include <QMutex>
#include <QImage>
#include "VideoRenderer.h"

using namespace	std;
using namespace dash::mpd;

class MediaEngine : public QObject
{
	Q_OBJECT

public:
	MediaEngine(IMPD *mpd, Frame *video);
	~MediaEngine();
	bool parseMPD(string url);
	IMPD* getMPD();
	bool start();
	bool createSegments(long currentSegmentNumber);
	void downloadSegments();
	void decodeSegments();
	void renderVideo();
	static void print(string string);
	void adjustPlayerSize();
	void setRepresentation(IRepresentation *representation);
	void setBandwidth(int bandwidth);
	void setVideoQuality(QString videoQuality);
	void showLoadingScreen();

private:
	Frame *video;
	IMPD *mpd;
	deque<ISegment*> segments;
	deque<ISegment*> segmentBuffer;
	SegmentFactory *segmentFactory;
	SegmentDownloader *segmentDownloader;
	SegmentDecoder *segmentDecoder;
	deque<QImage*> frameBuffer;
	QMutex frameBufferMutex;
	QWaitCondition frameBufferNotEmpty;
	QWaitCondition frameBufferNotFull;
	QMutex segmentBufferMutex;
	QWaitCondition segmentBufferNotEmpty;
	long currentSegmentNumber;
	bool decodingStarted = false;
	bool renderingStarted = false;
	vector<long> segmentFrameNumbers;
	VideoRenderer *videoRenderer;
	IRepresentation *representation;
	int bandwidth = 0;
	int width = 0;
	int height = 0;

public slots:
	void startDecoding();
	void startRendering();
	void saveNumberOfFrames(long numberOfFrames);

};
