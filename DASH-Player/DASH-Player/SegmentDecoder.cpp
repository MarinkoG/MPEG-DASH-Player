#include "SegmentDecoder.h"

SegmentDecoder::SegmentDecoder(deque<QImage*> *frameBuffer, QMutex *frameBufferMutex, QWaitCondition *frameBufferNotEmpty, QWaitCondition *frameBufferNotFull, deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty) :
	frameBuffer(frameBuffer),
	frameBufferMutex(frameBufferMutex),
	frameBufferNotEmpty(frameBufferNotEmpty),
	frameBufferNotFull(frameBufferNotFull),
	segmentBuffer(segmentBuffer),
	segmentBufferMutex(segmentBufferMutex),
	segmentBufferNotEmpty(segmentBufferNotEmpty)
{
}

SegmentDecoder::~SegmentDecoder()
{
	this->quit();
	this->wait();
}


static void print(string string) // for testing
{
	ofstream myfile;
	myfile.open("Log.txt", ios::app);
	myfile << string << endl;
	myfile.close();
}

static int read(void *data, uint8_t *buffer, int bufferSize)
{
	ISegment *segment = (ISegment*)data;
	int ret = segment->Read(buffer, bufferSize);
	return ret;
}

void SegmentDecoder::decode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *pkt)
{
	int ret;
	AVFrame *rgbFrame = NULL;
	int numBytes;
	uint8_t *buffer = NULL;
	struct SwsContext *sws_ctx = NULL;

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

		rgbFrame = av_frame_alloc();
		rgbFrame->width = frame->width;
		rgbFrame->height = frame->height;

		// Determine required buffer size and allocate buffer
		numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
		buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

		int s = av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);

		sws_ctx = sws_getContext(codecContext->width,
			codecContext->height,
			codecContext->pix_fmt,
			codecContext->width,
			codecContext->height,
			AV_PIX_FMT_RGB24,
			SWS_BICUBIC,
			NULL,
			NULL,
			NULL
		);

		// Convert the image from its native format to RGB
		sws_scale(sws_ctx, frame->data, frame->linesize, 0, codecContext->height, rgbFrame->data, rgbFrame->linesize);

		saveToBuffer(rgbFrame);

		av_free(buffer);
		av_frame_free(&rgbFrame);
	}
}

void SegmentDecoder::saveToBuffer(AVFrame *rgbFrame)
{
	frameBufferMutex->lock();
	if (frameBuffer->size() >= frameBufferSize)
	{
		frameBufferNotFull->wait(frameBufferMutex);
	}
	frameBufferMutex->unlock();

	QImage *image = new QImage(rgbFrame->width, rgbFrame->height, QImage::Format_RGB32);
	uint8_t *src = (uint8_t *)rgbFrame->data[0];

	for (size_t y = 0; y < rgbFrame->height; y++)
	{
		QRgb *scanLine = (QRgb *)image->scanLine(y);

		for (size_t x = 0; x < rgbFrame->width; x++)
			scanLine[x] = qRgb(src[3 * x], src[3 * x + 1], src[3 * x + 2]);

		src += rgbFrame->linesize[0];
	}
	numberOfFrames++;
	frameBuffer->push_back(image);

	frameBufferMutex->lock();
	if (frameBuffer->size() == 1)
	{
		emit framesDecoded();
		frameBufferNotEmpty->wakeAll();
	}
	frameBufferMutex->unlock();
}

void SegmentDecoder::run()
{
	QString result;
	AVFormatContext *formatContext = NULL;
	uint8_t *ioBuffer = NULL;
	AVCodecContext *codecContext = NULL;
	AVCodec *codec = NULL;
	int videoStream = -1;
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

		videoStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		codec = avcodec_find_decoder(formatContext->streams[videoStream]->codecpar->codec_id);

		codecContext = avcodec_alloc_context3(codec);
		if (!codecContext) {
			print("Could not allocate video codec context");
			return;
		}

		if ((avcodec_parameters_to_context(codecContext, formatContext->streams[videoStream]->codecpar)) < 0) {
			print("Could not copy codec context parameters");
			return;
		}

		if (avcodec_open2(codecContext, codec, NULL) < 0) {
			print("Could not open codec");
			return;
		}

		packet = av_packet_alloc();
		av_init_packet(packet);

		frame = av_frame_alloc();
		if (!frame) {
			print("Could not allocate video frame");
			return;
		}
		while (av_read_frame(formatContext, packet) >= 0)
		{
			if (packet->stream_index == videoStream)
			{
				decode(codecContext, frame, packet);
			}
			av_packet_unref(packet);
		}
		av_packet_free(&packet);
		decode(codecContext, frame, NULL);
		emit segmentDecoded(numberOfFrames);

		av_frame_free(&frame);
		avcodec_free_context(&codecContext);
		avformat_close_input(&formatContext);
		avio_context_free(&inputContext);
	}
}
