#include "MediaEngine.h"

MediaEngine::MediaEngine(Frame *video) : video(video)
{
}

MediaEngine::~MediaEngine()
{
}

bool MediaEngine::parseMPD(string uri)
{
	if (dash::IDASHManager *dashManager = CreateDashManager())
	{
		if (mpd = dashManager->Open((char*)uri.c_str()))
		{
			return true;
		}
	}
	return false;
}

IMPD * MediaEngine::getMPD()
{
	return this->mpd;
}
