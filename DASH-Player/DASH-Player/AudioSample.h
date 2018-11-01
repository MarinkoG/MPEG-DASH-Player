#pragma once
#include <iostream>
class AudioSample
{
public:
	AudioSample();
	AudioSample(char *data, long length);
	~AudioSample();
	char* getData();
	long getLength();

	char *data;
	long length = 8192;
private:
	/*
	char *data;
	long length = 0;
	*/
};


