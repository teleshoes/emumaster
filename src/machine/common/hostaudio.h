#ifndef HOSTAUDIO_H
#define HOSTAUDIO_H

#include <QObject>

#if defined(Q_OS_LINUX)
#include <pulse/simple.h>
#else
#include <QAudioOutput>
#endif

class HostAudio : public QObject {
    Q_OBJECT
	Q_PROPERTY(bool enable READ isEnabled WRITE setEnabled NOTIFY enableChanged)
	Q_PROPERTY(bool stereoEnable READ isStereoEnabled WRITE setStereoEnabled NOTIFY stereoEnableChanged)
	Q_PROPERTY(int sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged)
public:
    explicit HostAudio(QObject *parent = 0);
	~HostAudio();

	void open();
	void close();
	void write(const char *data, int size);

	bool isEnabled() const;
	void setEnabled(bool on);

	bool isStereoEnabled() const;
	void setStereoEnabled(bool on);

	int sampleRate() const;
	void setSampleRate(int rate);
signals:
	void enableChanged();
	void stereoEnableChanged();
	void sampleRateChanged();
private:
#if defined(Q_OS_LINUX)
	pa_simple *m_io;
#else
	QAudioOutput *m_audio;
	QIODevice *m_io;
#endif

	bool m_enable;
	bool m_stereoEnable;
	int m_sampleRate;
};

inline bool HostAudio::isEnabled() const
{ return m_enable; }
inline bool HostAudio::isStereoEnabled() const
{ return m_stereoEnable; }
inline int HostAudio::sampleRate() const
{ return m_sampleRate; }

#endif // HOSTAUDIO_H
