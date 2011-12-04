/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

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
inline int HostInputDevice::confIndex() const
{ return m_confIndex; }

#endif // HOSTINPUTDEVICE_H
