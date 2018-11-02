#include "AudioRenderer.h"


static void print(string string) // for testing
{
	ofstream myfile;
	myfile.open("renderLog.txt", ios::app);
	myfile << string << endl;
	myfile.close();
}


AudioRenderer::AudioRenderer(deque<AudioSample*> *audioSampleBuffer, QMutex *audioSampleBufferMutex, QWaitCondition *audioSampleBufferNotEmpty, QWaitCondition *audioSampleBufferNotFull) :
	audioSampleBuffer(audioSampleBuffer),
	audioSampleBufferMutex(audioSampleBufferMutex),
	audioSampleBufferNotEmpty(audioSampleBufferNotEmpty),
	audioSampleBufferNotFull(audioSampleBufferNotFull)
{
}

AudioRenderer::~AudioRenderer()
{
	this->quit();
	this->wait();
}

void AudioRenderer::run()
{
	init();
	int k = 0;
	AudioSample *audioSample;
	forever
	{
		audioSampleBufferMutex->lock();
		if (audioSampleBuffer->size() < 1)
		{
			audioSampleBufferNotEmpty->wait(audioSampleBufferMutex);
		}
		audioSampleBufferMutex->unlock();

		audioSample = *audioSampleBuffer->begin();
		
		if (++k<5) {
			for (size_t i = 0; i < audioSample->getLength(); i++)
			{
				print("frame: " + to_string(k) + " [" + to_string(audioSample->getData()[i]));
			}
		}
		writeToBuffer(audioSample->getData(), audioSample->getLength());
		msleep(1000/25);

		audioSampleBuffer->pop_front();
		audioSampleBufferMutex->lock();
		if (audioSampleBuffer->size() < audioSampleBufferSize)
		{
			audioSampleBufferNotFull->wakeAll();
		}
		audioSampleBufferMutex->unlock();

		delete audioSample;
	}
}

void AudioRenderer::setAudioFormat(QAudioFormat *format)
{
	this->format = format;
}

void AudioRenderer::writeToBuffer(const char *data, qint64 len)
{
	while (len > 0)
	{
		qint64 written = this->output->write(data, len);
		len -= written;
		data += written;
	}
}


void AudioRenderer::init()
{
	deviceInfo = QAudioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());
	f.setSampleRate(48000);
	f.setChannelCount(2);
	f.setSampleSize(16);
	f.setCodec("audio/pcm");
	f.setByteOrder(QAudioFormat::LittleEndian);
	f.setSampleType(QAudioFormat::SignedInt);
/*
	if (!deviceInfo.isFormatSupported(f))
	{
		f = deviceInfo.nearestFormat(f);
	}
	*/
	audioOutput = new QAudioOutput(deviceInfo, f, NULL);
	/*
	qreal linearVolume = QAudio::convertVolume(75 / qreal(100),
		QAudio::LogarithmicVolumeScale,
		QAudio::LinearVolumeScale);

	audioOutput->setVolume(linearVolume);

	*/
	this->output = audioOutput->start();
}

