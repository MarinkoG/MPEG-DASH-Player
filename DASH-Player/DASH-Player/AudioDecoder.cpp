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
	print(" duzina frame " + to_string((sizeof(frame->data[0]) / sizeof(char*))));
	AudioSample *audioSample = new AudioSample((char*)frame->data[0], frame->linesize[0]);

	audioSampleBuffer->push_back(audioSample);

	//print(" decoder-samplerate: " + to_string(frame->sample_rate) + " chanels: " + to_string(frame->channels) + " brojsamplova " + to_string(frame->nb_samples) + " datasize " + to_string(frame->linesize[0]));
	//print(" decoder-duzina: " + to_string(audioSample->getLength()) + " duzina frame " + to_string((sizeof(frame->data[0]) / sizeof(frame->data[0][0]))));
	msleep(30);
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
