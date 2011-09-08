#ifndef SNESMACHINE_H
#define SNESMACHINE_H

#include <imachine.h>
#include <QImage>

class SnesMachine : public IMachine {
    Q_OBJECT
public:
    explicit SnesMachine(QObject *parent = 0);

	QString setDisk(const QString &path);
	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	int fillAudioBuffer(char *stream, int streamSize);
	void setPadKey(PadKey key, bool state);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	QImage m_frame;
private:
};

#endif // SNESMACHINE_H
