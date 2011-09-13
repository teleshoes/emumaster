#ifndef SNESMACHINE_H
#define SNESMACHINE_H

#include <imachine.h>
#include <QImage>

typedef u8 bool8;
typedef u32 bool8_32;

class SnesMachine : public IMachine {
    Q_OBJECT
public:
    explicit SnesMachine(QObject *parent = 0);
	void reset();

	QString setDisk(const QString &path);
	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	int fillAudioBuffer(char *stream, int streamSize);
	void setPadKey(PadKey key, bool state);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	QImage m_frame;
};

#endif // SNESMACHINE_H
