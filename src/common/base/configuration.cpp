#include "configuration.h"
#include <QCoreApplication>

// TODO when configuration mismatch on load - show info as bubble

Configuration::Configuration(QObject *parent) :
	QObject(parent) {
}

QVariant Configuration::item(const QString &name, const QVariant &defaultValue) {
}

void Configuration::sl() {
}

void Configuration::setItem(const QString &name, const QVariant &value) {
}

void Configuration::setupAppInfo() {
	QCoreApplication::setOrganizationName("elemental");
	QCoreApplication::setOrganizationDomain("elemental-mk.blogspot.com");
	QCoreApplication::setApplicationName("emumaster");
	// TODO change on every release
	QCoreApplication::setApplicationVersion("0.1.0");
}
