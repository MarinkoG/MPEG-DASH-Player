#include "SegmentDownloader.h"

SegmentDownloader::SegmentDownloader(deque<ISegment*>* segments, deque<ISegment*>* segmentBuffer) :
	segments(segments),
	segmentBuffer(segmentBuffer)
{
}

SegmentDownloader::~SegmentDownloader()
{
	this->quit();
	this->wait();
}

void SegmentDownloader::run()
{
	QString result;
	for each (ISegment* segment in *segments)
	{
		DownloadObserver* downloadObserver = new DownloadObserver(&waitCondition);
		segment->AttachDownloadObserver(downloadObserver);
		segment->StartDownload();
		mutex.lock();
		waitCondition.wait(&mutex);
		segmentBuffer->push_back(segment);
		mutex.unlock();
	}
	emit downloadFinished(result);
}
