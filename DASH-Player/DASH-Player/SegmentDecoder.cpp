#include "SegmentDecoder.h"

SegmentDecoder::SegmentDecoder(QMutex* frameBufferLock, deque<QImage*>* frameBuffer, QMutex* segmentBufferLock, deque<ISegment*>* segmentBuffer) :
	frameBufferLock(frameBufferLock),
	frameBuffer(frameBuffer),
	segmentBufferLock(segmentBufferLock),
	segmentBuffer(segmentBuffer)
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

static int read(void* data, uint8_t *buffer, int bufferSize)
{
	ISegment* segment = (ISegment*)data;
	int ret = segment->Read(buffer, bufferSize);
	return ret;
}

static void decode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *pkt, int i)
{
	int ret;
	ret = avcodec_send_packet(codecContext, pkt);
	if (ret < 0) {
		print("Error sending a packet for decoding\n");
		return;
	}
	while (ret >= 0) {
		ret = avcodec_receive_frame(codecContext, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			print("Error during decoding\n");
			return;
		}

		AVFrame *rgbFrame = NULL;
		rgbFrame = av_frame_alloc();

		rgbFrame->width = frame->width;
		rgbFrame->height = frame->height;

		int numBytes;
		uint8_t *buffer = NULL;
		struct SwsContext *sws_ctx = NULL;

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

		QImage *image = new QImage(rgbFrame->width, rgbFrame->height, QImage::Format_RGB32);
		uint8_t *src = (uint8_t *)rgbFrame->data[0];

		for (size_t y = 0; y < rgbFrame->height; y++)
		{
			QRgb *scanLine = (QRgb *)image->scanLine(y);

			for (size_t x = 0; x < rgbFrame->width; x++)
				scanLine[x] = qRgb(src[3 * x], src[3 * x + 1], src[3 * x + 2]);

			src += rgbFrame->linesize[0];
		}

		std::string filename = "decodedImg\\slika" + std::to_string(i);
		filename += ".png";

		image->save(*(new QString(filename.c_str())));
		av_frame_free(&rgbFrame);
		image->~QImage();
                                                      	}
}

void saveSegment(ISegment* segment) // for testing
{

	int i = 1;

	ofstream file;
	string extension = ".mp4";
	string fileName = "after" + to_string(i);
	fileName = fileName + extension;
	file.open(fileName, ios::out | ios::binary);
	size_t len = 32768;
	uint8_t *p_data = new uint8_t[32768];
	int ret = 0;
	do
	{
		ret = segment->Read(p_data, len);
		if (ret > 0)
		{
			(&file)->write((char *)p_data, ret);
		}
	} while (ret > 0);
	file.close();
	i++;
}


void SegmentDecoder::run()
{
	QString result;
	AVFormatContext *formatContext = NULL;
	size_t bufferSize = 32768;
	uint8_t *ioBuffer = NULL;
	AVCodecContext *codecContext = NULL;
	AVCodec *codec = NULL;
	int videoStream = -1;
	AVFrame *frame = NULL;
	AVPacket *packet = NULL;
	AVIOContext *inputContext = NULL;
	int a = 0;
	a = segmentBuffer->size();
	print(std::to_string(a));
	for each (ISegment* segment in *segmentBuffer)
	{
		//saveSegment(segment);
		int i = 0;
		formatContext = avformat_alloc_context();
		ioBuffer = (uint8_t*)av_malloc(bufferSize);
		inputContext = avio_alloc_context(ioBuffer, bufferSize, 0, segment, &read, NULL, NULL);
		formatContext->pb = inputContext;
		formatContext->pb->seekable = 0;

		if (avformat_open_input(&formatContext, "", NULL, NULL) != 0)
		{
			return;
		}

		if (avformat_find_stream_info(formatContext, NULL) < 0)
		{
			return;
		}

		videoStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		codec = avcodec_find_decoder(formatContext->streams[videoStream]->codecpar->codec_id);

		codecContext = avcodec_alloc_context3(codec);
		if (!codecContext) {
			print("Could not allocate video codec context\n");
			return;
		}

		if ((avcodec_parameters_to_context(codecContext, formatContext->streams[videoStream]->codecpar)) < 0) {
			return;
		}

		/* For some codecs, such as msmpeg4 and mpeg4, width and height
		MUST be initialized there because this information is not
		available in the bitstream. */

		if (avcodec_open2(codecContext, codec, NULL) < 0) {
			print("Could not open codec\n");
			return;
		}

		packet = av_packet_alloc();
		av_init_packet(packet);

		frame = av_frame_alloc();
		if (!frame) {
			print("Could not allocate video frame\n");
			return;
		}


		while (av_read_frame(formatContext, packet) >= 0)
		{
			if (packet->stream_index == videoStream)
			{
				++i;
				decode(codecContext, frame, packet, i);
			}
			av_packet_unref(packet);
		}
		av_packet_free(&packet);
		decode(codecContext, frame, NULL, i);

		avformat_close_input(&formatContext);
		avcodec_free_context(&codecContext);
		av_frame_free(&frame);
	}
	emit decodingFinished(result);
}
