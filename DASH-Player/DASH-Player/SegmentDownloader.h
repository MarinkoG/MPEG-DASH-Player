#pragma once
#include <qobject.h>
#include <qthread.h>
#include <deque>
#include <ISegment.h>
#include "DownloadObserver.h"
#include <qmutex.h>
#include <qwaitcondition.h>

using namespace std;
using namespace dash::mpd;

class SegmentDownloader : public QThread
{
	Q_OBJECT

public:
	SegmentDownloader(deque<ISegment*> *segments, deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty);
	~SegmentDownloader();
	void run() override;

private:
	deque<ISegment*> *segments;
	deque<ISegment*> *segmentBuffer;
	QMutex segmentMutex;
	QWaitCondition downloadFinished;
	QWaitCondition *segmentBufferNotEmpty;
	QMutex *segmentBufferMutex;

signals:
	void segmentDownloaded();
};
