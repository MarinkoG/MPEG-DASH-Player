#pragma once

#include "Frame.h"
#include <IMPD.h>
#include <string.h>
#include <IDASHManager.h>
#include <qobject.h>
#include "libdash.h"

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

private:
	Frame *video;
	IMPD *mpd;
};
