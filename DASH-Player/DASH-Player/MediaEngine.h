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
#include "VideoDecoder.h"
#include <fstream>
#include <iostream>
#include <QMutex>
#include <QImage>
#include "VideoRenderer.h"
#include "AudioDecoder.h"
#include "AudioRenderer.h"

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
	void decodeVideoSegments();
	void renderVideo(double framerate);
	static void print(string string);
	void adjustPlayerSize();
	void setRepresentation(IRepresentation *representation);
	void setBandwidth(int bandwidth);
	void setVideoQuality(QString videoQuality);
	void showLoadingScreen();
	void decodeAudioSegments();

	void saveSegment();

private:
	Frame *video;
	IMPD *mpd;
	deque<ISegment*> videoSegments;
	deque<ISegment*> videoSegmentBuffer;
	SegmentFactory *segmentFactory;
	SegmentDownloader *segmentDownloader;
	VideoDecoder *videoDecoder;
	deque<QImage*> frameBuffer;
	QMutex frameBufferMutex;
	QWaitCondition frameBufferNotEmpty;
	QWaitCondition frameBufferNotFull;
	QMutex videoSegmentBufferMutex;
	QWaitCondition videoSegmentBufferNotEmpty;
	long currentSegmentNumber;
	bool decodingStarted = false;
	bool renderingStarted = false;
	vector<long> segmentFrameNumbers;
	VideoRenderer *videoRenderer;
	IRepresentation *representation;
	int bandwidth = 0;
	int width = 0;
	int height = 0;

	deque<ISegment*> audioSegments;
	deque<ISegment*> audioSegmentBuffer;
	SegmentDownloader *audioSegmentDownloader;
	QMutex audioSegmentBufferMutex;
	QWaitCondition audioSegmentBufferNotEmpty;

	AudioDecoder *audioDecoder;
	deque<AudioSample*> audioSampleBuffer;
	QMutex audioSampleBufferMutex;
	QWaitCondition audioSampleBufferNotEmpty;
	QWaitCondition audioSampleBufferNotFull;

	AudioRenderer *audioRenderer;

	
	

public slots:
	void startDecoding();
	void startRendering(double framerate);
	void startAudioRendering(QAudioFormat *format);
	void saveNumberOfFrames(long numberOfFrames);

};
