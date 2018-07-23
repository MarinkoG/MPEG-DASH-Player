#include "DownloadObserver.h"

DownloadObserver::DownloadObserver(QWaitCondition* waitCondition)
	: waitCondition(waitCondition)
{
}

DownloadObserver::~DownloadObserver()
{
}

void DownloadObserver::OnDownloadRateChanged(uint64_t bytesDownloaded)
{
}

void DownloadObserver::OnDownloadStateChanged(DownloadState state)
{
	downloadState = state;
	if (state == COMPLETED)
	{
		waitCondition->wakeAll();
	}
}
