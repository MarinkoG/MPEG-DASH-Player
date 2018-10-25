#include "MPDParser.h"

MPDParser::MPDParser()
{
}


MPDParser::~MPDParser()
{
}

bool MPDParser::parseMPD(string uri)
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

IMPD * MPDParser::getMPD()
{
	return this->mpd;
}

void MPDParser::separateAdaptationSets()
{
	if (mpd->GetPeriods().size() > 0)
	{
		period = this->mpd->GetPeriods().at(0);
		for each (IAdaptationSet *adaptationSet in period->GetAdaptationSets())
		{
			if ((&adaptationSet->GetContentType() != NULL) && (adaptationSet->GetContentType() == "video"))
			{
				videoAdaptationSets.push_back(adaptationSet);
			}
			else if ((&adaptationSet->GetMimeType() != NULL) && (adaptationSet->GetMimeType().find("video") != string::npos))
			{
				videoAdaptationSets.push_back(adaptationSet);
			}
			else
			{
				audioAdaptationSets.push_back(adaptationSet);
			}
		}
	}
}

void MPDParser::sortRepresentations()
{
	sort(representations.begin(), representations.end(), [](IRepresentation *first, IRepresentation *second)
	{
		return first->GetBandwidth() < second->GetBandwidth();
	});
}

void MPDParser::loadAtributes()
{
	separateAdaptationSets();
	representations = videoAdaptationSets.at(0)->GetRepresentation();
}

IRepresentation* MPDParser::getRepresentation(int bandwidth)
{
	for each (IRepresentation *representation in representations)
	{
		if (representation->GetBandwidth() == bandwidth)
		{
			return representation;
		}
	}
}

QStringList MPDParser::getVideoQualities()
{
	QStringList videoQualities;
	loadAtributes();
	sortRepresentations();
	for each (IRepresentation *representation in representations)
	{
		string videoQuality = to_string(representation->GetWidth()) + "x" + to_string(representation->GetHeight()) + " (" + to_string(representation->GetBandwidth()) + ")";
		videoQualities.push_back(QString(videoQuality.c_str()));
	}
	return videoQualities;
}
