#include "VideoDecoder.h"

VideoDecoder::VideoDecoder(deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty, deque<QImage*> *frameBuffer, QMutex *frameBufferMutex, QWaitCondition *frameBufferNotEmpty, QWaitCondition *frameBufferNotFull) :
	Decoder(segmentBuffer, segmentBufferMutex, segmentBufferNotEmpty),
	frameBuffer(frameBuffer),
	frameBufferMutex(frameBufferMutex),
	frameBufferNotEmpty(frameBufferNotEmpty),
	frameBufferNotFull(frameBufferNotFull)
{
	setMediaType(AVMEDIA_TYPE_VIDEO);
}

VideoDecoder::~VideoDecoder()
{
}

void VideoDecoder::saveToBuffer(AVCodecContext *codecContext, AVFrame *frame)
{
	AVFrame *rgbFrame = NULL;
	int numBytes;
	uint8_t *buffer = NULL;
	struct SwsContext *sws_ctx = NULL;

	frameBufferMutex->lock();
	if (frameBuffer->size() >= frameBufferSize)
	{
		frameBufferNotFull->wait(frameBufferMutex);
	}
	frameBufferMutex->unlock();

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

	QImage *image = new QImage(rgbFrame->width, rgbFrame->height, QImage::Format_RGB32);
	uint8_t *src = (uint8_t *)rgbFrame->data[0];

	for (size_t y = 0; y < rgbFrame->height; y++)
	{
		QRgb *scanLine = (QRgb *)image->scanLine(y);

		for (size_t x = 0; x < rgbFrame->width; x++)
			scanLine[x] = qRgb(src[3 * x], src[3 * x + 1], src[3 * x + 2]);

		src += rgbFrame->linesize[0];
	}
	frameBuffer->push_back(image);

	frameBufferMutex->lock();
	if (frameBuffer->size() == 1)
	{
		emit framesDecoded(av_q2d(codecContext->framerate));
		frameBufferNotEmpty->wakeAll();
	}
	frameBufferMutex->unlock();

	av_free(buffer);
	av_frame_free(&rgbFrame);
}
