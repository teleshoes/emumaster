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
#include <QCoreApplication>

// TODO when configuration mismatch on load - show info as bubble
// TODO hard config

Configuration::Configuration(QObject *parent) :
	QObject(parent) {
}

void Configuration::setItem(const QString &name, const QVariant &value) {
}

QVariant Configuration::item(const QString &name, const QVariant &defaultValue) {
	return defaultValue;
}

void Configuration::sl() {
}

void Configuration::setupAppInfo() {
	QCoreApplication::setOrganizationName("elemental");
	QCoreApplication::setOrganizationDomain("elemental-mk.blogspot.com");
	QCoreApplication::setApplicationName("emumaster");
	// TODO change on every release
	QCoreApplication::setApplicationVersion("0.1.0");
}
