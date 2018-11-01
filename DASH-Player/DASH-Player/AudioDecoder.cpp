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
	audioSample = new AudioSample();
	int i,ch;
	int data_size = av_get_bytes_per_sample(codecContext->sample_fmt);
	if (data_size < 0) {
		/* This should not occur, checking just for paranoia */
		print("Failed to calculate data size\n");
	}

	audioSample->data = (char*)malloc(frame->linesize[0]);
	print("linesize[0]= " + to_string(frame->linesize[0]) + "linesize[1]= " + to_string(frame->linesize[1]) + "codecContext->channels= " + to_string(codecContext->channels));
	//memcpy(audioSample->data, data, this->length);
	//FILE *outfile = fopen("audioSam.mpa", "a");
	
	for (i = 0; i < frame->nb_samples; i++)
		for (ch = 0; ch < codecContext->channels; ch++)
			memcpy(audioSample->data+data_size*i, frame->data[ch] + data_size * i, data_size);
			//fwrite(frame->data[ch] + data_size * i, 1, data_size, outfile);

	//fclose(outfile);
	//memcpy(audioSample->data, frame->data[ch] + data_size * i, data_size);

	//long size = av_get_bytes_per_sample(codecContext->sample_fmt);
	//audioSample = new AudioSample((char*)frame->data[0], frame->linesize[0]);

	audioSampleBuffer->push_back(audioSample);
	//print(" decoder-samplerate: " + to_string(frame->sample_rate) + " chanels: " + to_string(frame->channels) + " brojsamplova " + to_string(frame->nb_samples) + " datasize " + to_string(frame->linesize[0]) + " size " + to_string(size));
	//print(" decoder-duzina: " + to_string(audioSample->getLength()) + " duzina frame " + to_string((sizeof(frame->data[0]) / sizeof(frame->data[0][0]))));
	//msleep(30);
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
