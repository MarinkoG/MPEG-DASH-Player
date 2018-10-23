#include "SegmentDownloader.h"

SegmentDownloader::SegmentDownloader(deque<ISegment*> *segments, deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty) :
	segments(segments),
	segmentBuffer(segmentBuffer),
	segmentBufferMutex(segmentBufferMutex),
	segmentBufferNotEmpty(segmentBufferNotEmpty)
{
}

SegmentDownloader::~SegmentDownloader()
{
	this->quit();
	this->wait();
}

void SegmentDownloader::run()
{
	for each (ISegment *segment in *segments)
	{
		DownloadObserver *downloadObserver = new DownloadObserver(&downloadFinished);

		segment->AttachDownloadObserver(downloadObserver);
		segment->StartDownload();

		segmentMutex.lock();
		downloadFinished.wait(&segmentMutex);
		segmentMutex.unlock();

		segmentBufferMutex->lock();
		segmentBuffer->push_back(segment);
		segmentBufferNotEmpty->wakeAll();
		segmentBufferMutex->unlock();

		emit segmentDownloaded();
	}
}
