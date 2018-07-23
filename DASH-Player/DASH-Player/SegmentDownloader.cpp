#include "SegmentDownloader.h"

SegmentDownloader::SegmentDownloader(deque<ISegment*>* segments, deque<ISegment*>* downloadedSegments) :
	segments(segments),
	downloadedSegments(downloadedSegments)
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
		downloadedSegments->push_back(segment);
		mutex.unlock();
	}
	emit downloadFinished(result);
}
