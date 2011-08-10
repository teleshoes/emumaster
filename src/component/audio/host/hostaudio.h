#ifndef HOSTAUDIO_H
#define HOSTAUDIO_H

#include <QAudioFormat>
#include <QAudioOutput>
#include <QIODevice>
#include <QObject>

#if defined(HOSTAUDIO_PROJECT)
#define HOSTAUDIO_EXPORT Q_DECL_EXPORT
#else
#define HOSTAUDIO_EXPORT Q_DECL_IMPORT
#endif

class HOSTAUDIO_EXPORT HostAudio : public QObject {
    Q_OBJECT
public:
	explicit HostAudio(QObject *parent = 0);
	void setFormat(const QAudioFormat &fmt);
	void write(const char *data, int size);
	QList<int> supportedSampleRates() const;
private:
	QAudioOutput *m_audio;
	QIODevice *m_io;
};

#endif // HOSTAUDIO_H
