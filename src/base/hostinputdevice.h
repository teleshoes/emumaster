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

#include "base_global.h"
#include <QObject>
#include <QStringList>

class HostInputDevice : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name CONSTANT)
	Q_PROPERTY(int confIndex READ confIndex WRITE setConfIndex NOTIFY confChanged)
public:
	explicit HostInputDevice(const QString &name, QObject *parent = 0);
	QString name() const;

	int confIndex() const;
	void setConfIndex(int index);

	virtual void update(int *data) = 0;

	void setGlobalConfigurationName(const QString &name);
	void updateConfFromGlobalConfiguration();
signals:
	void confChanged();
private:
	QString m_name;
	int m_confIndex;

	QString m_globalConfName;
};

inline QString HostInputDevice::name() const
{ return m_name; }
inline int HostInputDevice::confIndex() const
{ return m_confIndex; }

#endif // HOSTINPUTDEVICE_H
