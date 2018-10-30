#pragma once
#include <iostream>
class AudioSample
{
public:
	AudioSample(char *data, long length);
	~AudioSample();
	char* getData();
	long getLength();

private:
	char *data;
	long length = 0;
};


