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

using namespace	std;
using namespace dash::mpd;

class MediaEngine : public QObject
{
	Q_OBJECT

public:
	MediaEngine(Frame *video);
	~MediaEngine();
	bool parseMPD(string url);
	IMPD* getMPD();
	bool start();
	bool createSegments(long currentSegmentNumber);
	void downloadSegments();
	void decodeSegments();
	static void print(string string);

private:
	Frame *video;
	IMPD *mpd;
	deque<ISegment*> segments;
	deque<ISegment*> segmentBuffer;
	SegmentFactory* segmentFactory;
	SegmentDownloader *segmentDownloader;
	SegmentDecoder *segmentDecoder;
	deque<QImage*> frameBuffer;
	QMutex frameBufferMutex;
	QWaitCondition frameBufferNotEmpty;
	QMutex segmentBufferMutex;
	QWaitCondition segmentBufferNotEmpty;
	long currentSegmentNumber;
	bool decodingStarted = false;
	vector<long> segmentFrameNumbers;

public slots:
	void startDecoding();
	void saveNumberOfFrames(long numberOfFrames);
};
