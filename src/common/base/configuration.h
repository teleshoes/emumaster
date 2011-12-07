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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QVariant>

class Configuration : public QObject {
    Q_OBJECT
public:
	static void setupAppInfo();

    explicit Configuration(QObject *parent = 0);
	QVariant item(const QString &name,
				  const QVariant &defaultValue = QVariant());
	void setItem(const QString &name, const QVariant &value);
	void setItemHard(const QString &name, const QVariant &value);

	void sl();
private:
	QHash<QString, QVariant> m_data;
};

#endif // CONFIGURATION_H
