#include "AudioDevice.h"

extern "C" {
#include "gba.h"
#include "gba-audio.h"
#include "gba-thread.h"
}

using namespace QGBA;

AudioDevice::AudioDevice(QObject* parent)
	: QIODevice(parent)
	, m_context(nullptr)
{
	setOpenMode(ReadOnly);
}

void AudioDevice::setFormat(const QAudioFormat& format) {
	if (!GBAThreadHasStarted(m_context)) {
		return;
	}
	// TODO: make this thread-safe
	m_ratio = GBAAudioCalculateRatio(&m_context->gba->audio, 60, format.sampleRate());
}

void AudioDevice::setInput(GBAThread* input) {
	m_context = input;
}

qint64 AudioDevice::readData(char* data, qint64 maxSize) {
	if (maxSize > 0xFFFFFFFF) {
		maxSize = 0xFFFFFFFF;
	}

	if (!m_context->gba) {
		return 0;
	}

	return GBAAudioResampleNN(&m_context->gba->audio, m_ratio, &m_drift, reinterpret_cast<GBAStereoSample*>(data), maxSize / sizeof(GBAStereoSample)) * sizeof(GBAStereoSample);
}

qint64 AudioDevice::writeData(const char*, qint64) {
	return 0;
}
