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
	QString init();
	void shutdown();

	void reset();

	QString setDisk(const QString &path);
	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	int fillAudioBuffer(char *stream, int streamSize);
	void setPadKeys(int pad, int keys);

	void sync(int width, int height);

	QImage m_frame;
protected:
	void sl();
};

extern SnesMachine snesMachine;

#endif // SNESMACHINE_H
