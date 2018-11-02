#include "AudioDecoder.h"

#include <fstream>
#include <iostream>
#include <string.h>

static void print(string string) // for testing
{
	ofstream myfile;
	myfile.open("decoderLog.txt", ios::app);
	myfile << string << endl;
	myfile.close();
}

AudioDecoder::AudioDecoder(deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty, deque<AudioSample*> *audioSampleBuffer, QMutex *audioSampleBufferMutex, QWaitCondition *audioSampleBufferNotEmpty, QWaitCondition *audioSampleBufferNotFull):
	Decoder(segmentBuffer, segmentBufferMutex, segmentBufferNotEmpty),
	audioSampleBuffer(audioSampleBuffer),
	audioSampleBufferMutex(audioSampleBufferMutex),
	audioSampleBufferNotEmpty(audioSampleBufferNotEmpty),
	audioSampleBufferNotFull(audioSampleBufferNotFull)
{
	setMediaType(AVMEDIA_TYPE_AUDIO);
}

AudioDecoder::~AudioDecoder()
{
	this->quit();
	this->wait();
}

void AudioDecoder::saveToBuffer(AVCodecContext *codecContext, AVFrame *frame)
{
	audioSampleBufferMutex->lock();
	if (audioSampleBuffer->size() >= audioSampleBufferSize)
	{
		audioSampleBufferNotFull->wait(audioSampleBufferMutex);
	}
	audioSampleBufferMutex->unlock();
	
	if (codecContext->frame_number<5) {
		for (size_t i = 0; i < frame->linesize[0]; i++)
		{
			print("frame: " + to_string(codecContext->frame_number) +  " [" + to_string(frame->data[0][i]));
		}
	}


	audioSample = new AudioSample((char*)frame->data[0], frame->linesize[0]);
	audioSampleBuffer->push_back(audioSample);

	audioSampleBufferMutex->lock();
	if (audioSampleBuffer->size() == 1)
	{

		QAudioFormat *format = new QAudioFormat();
		format->setSampleRate(codecContext->sample_rate);
		format->setChannelCount(codecContext->channels);
		format->setSampleSize(16);
		format->setCodec("audio/pcm");
		format->setByteOrder(QAudioFormat::LittleEndian);
		format->setSampleType(QAudioFormat::SignedInt);


		emit framesDecoded(format);
		audioSampleBufferNotEmpty->wakeAll();
	}
	audioSampleBufferMutex->unlock();

}
