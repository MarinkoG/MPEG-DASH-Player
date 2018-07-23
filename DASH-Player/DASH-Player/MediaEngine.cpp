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

bool MediaEngine::start()
{
	createSegments(mpd);
	downloadSegments();
	return false;
}

bool MediaEngine::createSegments(IMPD * mpd)
{
	segmentFactory = new SegmentFactory(mpd);
	segments = segmentFactory->createSegments();
	return true;
}

bool MediaEngine::downloadSegments()
{
	segmentDownloader = new SegmentDownloader(&segments, &downloadedSegments);
	segmentDownloader->start();
	return true;
}

void MediaEngine::print(string string) // for testing
{
	ofstream myfile;
	myfile.open("Log.txt", ios::app);
	myfile << string << endl;
	myfile.close();
}

void MediaEngine::saveSegment() // for testing
{
	int i = 1;
	for each (ISegment* segment in downloadedSegments)
	{
		ofstream file;
		string extension = ".mp4";
		string fileName = "video" + to_string(i);
		fileName = fileName + extension;
		file.open(fileName, ios::out | ios::binary);
		size_t len = 32768;
		uint8_t *p_data = new uint8_t[32768];
		int ret = 0;
		do
		{
			ret = segment->Read(p_data, len);
			if (ret > 0)
			{
				(&file)->write((char *)p_data, ret);
			}
		} while (ret > 0);
		file.close();
		i++;
	}
}
