#pragma once

#include <IDownloadObserver.h>
#include <qobject.h>
#include <qstring.h>
#include <qwaitcondition.h>

using namespace dash::network;
using namespace std;

class DownloadObserver : public QObject, public IDownloadObserver
{
	Q_OBJECT

public:
	DownloadObserver(QWaitCondition* waitCondition);
	~DownloadObserver();

	// Inherited via IDownloadObserver
	virtual void OnDownloadRateChanged(uint64_t bytesDownloaded) override;
	virtual void OnDownloadStateChanged(DownloadState state) override;

private:
	DownloadState downloadState;
	QWaitCondition* waitCondition;
};
