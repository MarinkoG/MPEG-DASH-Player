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

static int read(void* data, uint8_t * buffer, int bufferSize)
{
	ISegment* segment = (ISegment*)data;
	int ret = segment->Read(buffer, bufferSize);
	return ret;
}


void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
	FILE *pFile;
	char szFilename[32];
	int  y;

	// Open file
	sprintf(szFilename, "frame%d.ppm", iFrame);
	pFile = fopen(szFilename, "wb");
	if (pFile == NULL)
		return;

	// Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for (y = 0; y<height; y++)
		fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);

	// Close file
	fclose(pFile);
}

static void decode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *pkt)
{
	//char buf[1024];
	int ret;

	//printf("avcodec_send_packet BEFORE\n");
	ret = avcodec_send_packet(codecContext, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");
		exit(1);
	}
	//printf("avcodec_send_packet OK %d\n", ret);

	while (ret >= 0) {
		//printf("avcodec_receive_frame BEFORE\n");
		ret = avcodec_receive_frame(codecContext, frame);

		//printf("avcodec_receive_frame AFTER %d\n", ret);

		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}

		printf("\nsaving frame %3d\n", codecContext->frame_number);
		fflush(stdout);

		/* the picture is allocated by the decoder. no need to
		free it */
		//snprintf(buf, sizeof(buf), "%s-%d.bmp", filename, dec_ctx->frame_number);

		AVFrame *rgbFrame = NULL;
		rgbFrame = av_frame_alloc();

		printf("frame HEIGHT %d Width %d\n", frame->width, frame->height);


		rgbFrame->width = frame->width;
		rgbFrame->height = frame->height;

		printf("RGB HEIGHT %d Width %d\n", rgbFrame->width, rgbFrame->height);

		int numBytes;
		uint8_t *buffer = NULL;
		struct SwsContext *sws_ctx = NULL;

		// Determine required buffer size and allocate buffer
		numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
		buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
		printf("numBytes %d", numBytes);

		int s = av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
		printf("\noriginal PIX_format %d, %d   novi PIX_format\n", codecContext->pix_fmt, AV_PIX_FMT_RGB24);
		printf("av_image_fill_arrays %d\n", s);
		// initialize SWS context for software scaling

		printf("RGB HEIGHT %d Width %d\n", rgbFrame->width, rgbFrame->height);
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


		printf("RGB AFTER HEIGHT %d Width %d\n", rgbFrame->width, rgbFrame->height);


		QImage *image = new QImage(rgbFrame->width, rgbFrame->height, QImage::Format_RGB32);
		uint8_t *src = (uint8_t *)rgbFrame->data[0];

		for (size_t y = 0; y < rgbFrame->height; y++)
		{
			QRgb *scanLine = (QRgb *)image->scanLine(y);

			for (size_t x = 0; x < rgbFrame->width; x++)
				scanLine[x] = qRgb(src[3 * x], src[3 * x + 1], src[3 * x + 2]);

			src += rgbFrame->linesize[0];
		}

		std::string filename = "slika" + std::to_string(i);
		filename += ".png";

		image->save(*(new QString(filename.c_str())));
		av_frame_free(&rgbFrame);
	}
}

void SegmentDecoder::run()
{
	QString result;
	AVFormatContext *formatContext = NULL;
	//AVIOContext *avioContext = NULL;
	int bufferSize = 32768;
	uint8_t *ioBuffer;
	AVCodecContext *codecContext = NULL;
	AVCodec *codec = NULL;
	int videoStream = -1;
	AVFrame *frame = NULL;

	for each (ISegment* segment in *segmentBuffer)
	{
		int i = 0;
		formatContext = avformat_alloc_context();
		ioBuffer = (uint8_t*)av_malloc(bufferSize);
		formatContext->pb = avio_alloc_context(ioBuffer, bufferSize, 0, segment, read, NULL, NULL);
		formatContext->pb->seekable = 0;


		//printf("asign to format contx\n");


	if (avformat_open_input(&formatContext, NULL, NULL, NULL) != 0)
	{
		return;
	}

	if (avformat_find_stream_info(formatContext, NULL) < 0)
	{
		return;
	}

	videoStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	codec = avcodec_find_decoder(formatContext->streams[videoStream]->codecpar->codec_id);

	//printf("Codec id %d\n", codec->id);

	codecContext = avcodec_alloc_context3(codec);
	if (!codecContext) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}

	if ((avcodec_parameters_to_context(c, formatContext->streams[videoStream]->codecpar)) < 0) {
		return;
	}

	/* For some codecs, such as msmpeg4 and mpeg4, width and height
	MUST be initialized there because this information is not
	available in the bitstream. */

	/* open it */
	if (avcodec_open2(codecContext, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}

	AVPacket *packet;


	packet = av_packet_alloc();
	av_init_packet(packet);

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}


	while (av_read_frame(formatContext, packet) >= 0)
	{
		// Is this a packet from the video stream?
		if (packet->stream_index == videoStream && ++i<=5)
		{
			decode(c, frame, packet);
		}
		//av_packet_unref(packet);
	}
	av_packet_free(&packet);
	// Free the packet that was allocated by av_read_frame


/* flush the decoder */
	decode(c, frame, NULL);

	avformat_close_input(&formatContext);
	avcodec_free_context(&c);
	av_frame_free(&frame);


	printf("asign to format contx\n");


	if (avformat_open_input(&formatContext, NULL, NULL, NULL) != 0)
	{
		return 0;
		printf("avformat_open_input\n");
	}

	if (avformat_find_stream_info(formatContext, NULL) < 0)
	{
		return 0;
	}

	AVCodecContext *codecContext = NULL;
	AVCodec *codec = NULL;

	videoStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	codec = avcodec_find_decoder(formatContext->streams[videoStream]->codecpar->codec_id);

	printf("Codec id %d\n", codec->id);

	c = avcodec_alloc_context3(codec);
	if (!c) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}

	if ((ret = avcodec_parameters_to_context(c, formatContext->streams[videoStream]->codecpar)) < 0) {
		return ret;
	}

	/* For some codecs, such as msmpeg4 and mpeg4, width and height
	MUST be initialized there because this information is not
	available in the bitstream. */

	/* open it */
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}

	//printf("Codec OPEN id %d\n", codec->id);


	AVPacket *packet;


	packet = av_packet_alloc();
	av_init_packet(packet);

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}


	while (av_read_frame(formatContext, packet) >= 0)
	{
		// Is this a packet from the video stream?
		if (packet->stream_index == videoStream && ++i <= 5)
		{
			decode(c, frame, packet, outfilename);
		}
		//av_packet_unref(packet);
	}
	av_packet_free(&packet);
	// Free the packet that was allocated by av_read_frame


	/* flush the decoder */
	decode(c, frame, NULL, outfilename);

	avformat_close_input(&formatContext);
	avcodec_free_context(&c);
	av_frame_free(&frame);




			// Free the packet that was allocated by av_read_frame
			av_free_packet(&packet);

			// Free the RGB image
			av_free(buffer);
			av_free(pFrameRGB);

			// Free the YUV frame
			av_free(avFrame);

			// Close the codecs
			avcodec_close(pCodecCtx);
			avcodec_close(pCodecCtxOrig);

			// Close the video file
			avformat_close_input(&formatCtx);

		}
	}
	emit decodingFinished(result);
}
