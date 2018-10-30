#include "AudioSample.h"

AudioSample::AudioSample(char * data, long length):
	data(data),
	length(length)
{
	this->data = (char*)malloc(this->length);
	memcpy(this->data, data, this->length);
}

AudioSample::~AudioSample()
{
	free(this->data);
}

char * AudioSample::getData()
{
	return this->data;
}

long AudioSample::getLength()
{
	return this->length;
}
