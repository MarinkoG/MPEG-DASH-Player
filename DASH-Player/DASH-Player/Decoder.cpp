#include "Decoder.h"

Decoder::Decoder(deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty) :
	segmentBuffer(segmentBuffer),
	segmentBufferMutex(segmentBufferMutex),
	segmentBufferNotEmpty(segmentBufferNotEmpty)
{
}

Decoder::~Decoder()
{
	this->quit();
	this->wait();
}


void Decoder::print(string string) // for testing
{
	ofstream myfile;
	myfile.open("decoderLog.txt", ios::app);
	myfile << string << endl;
	myfile.close();
}

static int read(void *data, uint8_t *buffer, int bufferSize)
{
	ISegment *segment = (ISegment*)data;
	int ret = segment->Read(buffer, bufferSize);
	return ret;
}

void Decoder::decode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *pkt)
{
	int ret;
	int numBytes;
	uint8_t *buffer = NULL;

	ret = avcodec_send_packet(codecContext, pkt);
	if (ret < 0) {
		print("Error sending a packet for decoding");
		return;
	}
	while (ret >= 0) {
		ret = avcodec_receive_frame(codecContext, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			print("Error during decoding");
			return;
		}
		numberOfFrames++;
		saveToBuffer(codecContext, frame);
	}
}

void Decoder::run()
{
	av_register_all();
	QString result;
	AVFormatContext *formatContext = NULL;
	uint8_t *ioBuffer = NULL;
	AVCodecContext *codecContext = NULL;
	AVCodec *codec = NULL;
	AVFrame *frame = NULL;
	AVPacket *packet = NULL;
	AVIOContext *inputContext = NULL;

	forever
	{
		ISegment *segment = NULL;
		segmentBufferMutex->lock();
		if (segmentBuffer->size() == 0)
		{
			segmentBufferNotEmpty->wait(segmentBufferMutex);
		}
		segmentBufferMutex->unlock();

		segment = *segmentBuffer->begin();
		segmentBuffer->pop_front();

		formatContext = avformat_alloc_context();
		ioBuffer = (uint8_t*)av_malloc(bufferSize);
		inputContext = avio_alloc_context(ioBuffer, bufferSize, 0, segment, &read, NULL, NULL);
		formatContext->pb = inputContext;
		formatContext->pb->seekable = 0;
		if (avformat_open_input(&formatContext, "", NULL, NULL) != 0)
		{
			print("Could not open format context");
			return;
		}

		if (avformat_find_stream_info(formatContext, NULL) < 0)
		{
			print("Could not find stream");
			return;
		}

		stream = av_find_best_stream(formatContext, mediaType, -1, -1, NULL, 0);
		codec = avcodec_find_decoder(formatContext->streams[stream]->codecpar->codec_id);

		codecContext = avcodec_alloc_context3(codec);
		if (!codecContext) {
			print("Could not allocate codec context");
			return;
		}

		if ((avcodec_parameters_to_context(codecContext, formatContext->streams[stream]->codecpar)) < 0) {
			print("Could not copy codec context parameters");
			return;
		}

		if (avcodec_open2(codecContext, codec, NULL) < 0) {
			print("Could not open codec");
			return;
		}
		int got_frame = 1;
		packet = av_packet_alloc();
		av_init_packet(packet);

		frame = av_frame_alloc();
		if (!frame) {
			print("Could not allocate frame");
			return;
		}
		while (av_read_frame(formatContext, packet) >= 0)
		{
			//decode(codecContext, frame, packet);
			//av_packet_unref(packet);
			avcodec_decode_audio4(codecContext, frame, &got_frame, packet);
			if (got_frame)
			{
				saveToBuffer(codecContext, frame);
			}
		}
		av_packet_free(&packet);
		//decode(codecContext, frame, NULL);
		emit segmentDecoded(numberOfFrames);

		av_frame_free(&frame);
		avcodec_free_context(&codecContext);
		avformat_close_input(&formatContext);
		avio_context_free(&inputContext);
	}
}

void Decoder::saveToBuffer(AVCodecContext *codecContext, AVFrame *frame)
{
}

void Decoder::setMediaType(AVMediaType mediaType)
{
	this->mediaType = mediaType;
}
