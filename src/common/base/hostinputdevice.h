#ifndef HOSTINPUTDEVICE_H
#define HOSTINPUTDEVICE_H

#include <QObject>
#include <QStringList>

class HostInputDevice : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name CONSTANT)
	Q_PROPERTY(QString conf READ conf WRITE setConf NOTIFY confChanged)
	Q_PROPERTY(QStringList confList READ confList CONSTANT)
public:
	// TODO rename keys
	enum PadKey {
		Right_PadKey	= (1 <<  0),
		Down_PadKey		= (1 <<  1),
		Up_PadKey		= (1 <<  2),
		Left_PadKey		= (1 <<  3),

		A_PadKey		= (1 <<  4),
		B_PadKey		= (1 <<  5),
		X_PadKey		= (1 <<  6),
		Y_PadKey		= (1 <<  7),

		L_PadKey		= (1 <<  8),
		R_PadKey		= (1 <<  9),
		L2_PadKey		= (1 <<  8),
		R2_PadKey		= (1 <<  9),

		Start_PadKey	= (1 << 10),
		Select_PadKey	= (1 << 11)
	};

	static int *padOffset(int *data, int pad);
	static int *mouseOffset(int *data, int mouse);

	explicit HostInputDevice(const QString &name, QObject *parent = 0);
	QString name() const;

	int confIndex() const;
	QString conf() const;
	void setConf(const QString &type);
	QStringList confList() const;

	virtual void update(int *data) = 0;
signals:
	void confChanged();
protected:
	void setConfList(const QStringList &list);
private:
	QString m_name;
	int m_confIndex;
	QStringList m_confList;
};

inline QString HostInputDevice::name() const
{ return m_name; }
inline QStringList HostInputDevice::confList() const
{ return m_confList; }

inline int *HostInputDevice::padOffset(int *data, int pad)
{ Q_ASSERT(pad >= 0 && pad < 2); return &data[pad*4]; }
inline int *HostInputDevice::mouseOffset(int *data, int mouse)
{ Q_ASSERT(mouse >= 0 && mouse < 2); return &data[(mouse+2)*4]; }

#endif // HOSTINPUTDEVICE_H
