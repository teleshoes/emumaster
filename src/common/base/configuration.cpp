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

#include "configuration.h"
#include "imachine.h"
#include <QCoreApplication>

// TODO when hard configuration mismatch on load - show info as bubble
// TODO hard config

Configuration *Configuration::m_instance = 0;

Configuration::Configuration() {
}

void Configuration::setItem(const QString &name, const QVariant &value) {
	m_data[name] = value;
}

QVariant Configuration::item(const QString &name, const QVariant &defaultValue) {
	if (!m_data.contains(name))
		return defaultValue;
	return m_data.value(name);
}

void Configuration::sl() {
	emsl.begin("conf");
	emsl.var("data", m_data);
	emsl.end();
}

void Configuration::setupAppInfo() {
	QCoreApplication::setOrganizationName("elemental");
	QCoreApplication::setOrganizationDomain("elemental-mk.blogspot.com");
	QCoreApplication::setApplicationName("emumaster");
	// TODO change on every release
	QCoreApplication::setApplicationVersion("0.1.0");
}

Configuration *Configuration::instance() {
	if (!m_instance)
		m_instance = new Configuration();
	return m_instance;
}
