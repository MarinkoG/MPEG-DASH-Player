#include "SegmentFactory.h"

SegmentFactory::SegmentFactory()
{
}

SegmentFactory::SegmentFactory(IMPD * mpd):
	mpd(mpd)
{
}

SegmentFactory::~SegmentFactory()
{
}

IRepresentation* SegmentFactory::getRepresentation(int bandwidth)
{
	adaptationSet = getAdaptationSet("video");
	vector <IRepresentation*> representations = adaptationSet->GetRepresentation();

	if (bandwidth != 0)
	{
		for each (IRepresentation* representation in representations)
		{
			if (representation->GetBandwidth() == bandwidth)
			{
				return representation;
			}
		}
	}
	else
	{
		sort(representations.begin(), representations.end(), [](IRepresentation* first, IRepresentation* second)
		{
			return first->GetBandwidth() < second->GetBandwidth();
		});

		return representations.front();
	}
}

IAdaptationSet * SegmentFactory::getAdaptationSet(string type)
{
	if (mpd->GetPeriods().size() > 0)
	{
		period = this->mpd->GetPeriods().at(0);
		for each (IAdaptationSet *adaptationSet in period->GetAdaptationSets())
		{
			if ((&adaptationSet->GetContentType() != NULL) && (adaptationSet->GetContentType() == type))
			{
				return adaptationSet;
			}
			if ((&adaptationSet->GetMimeType() != NULL) && (adaptationSet->GetMimeType().find(type) != string::npos))
			{
				return adaptationSet;
			}
		}
	}
	return nullptr;
}

void SegmentFactory::findSegmentFolderPath()
{
	if (mpd->GetBaseUrls().size() > 0) //alternative segment location
	{
		baseUrls.push_back(mpd->GetBaseUrls().at(0));
	}
	else
	{
		baseUrls.push_back(mpd->GetMPDPathBaseUrl());
	}

	if (period->GetBaseURLs().size() > 0) // check if period contains base urls
	{
		baseUrls.push_back(period->GetBaseURLs().at(0));
	}

	if (adaptationSet->GetBaseURLs().size() > 0) // check if adaptationSet contains base urls
	{
		baseUrls.push_back(adaptationSet->GetBaseURLs().at(0));
	}
}

deque<ISegment*> SegmentFactory::getSegments()
{
	return segments;
}

deque<ISegment*> SegmentFactory::createSegments(int bandwidth)
{
	representation = getRepresentation(bandwidth);
	findSegmentFolderPath();

	if (representation->GetBaseURLs().size() > 0) // if representation has baseUrl there is only 1 segment
	{
		segments.push_back(representation->GetBaseURLs().at(0)->ToMediaSegment(baseUrls));
	}
	return segments;
}
