#include "configuration.h"

// TODO when configuration mismatch on load - show info as bubble

Configuration::Configuration(QObject *parent) :
    QObject(parent)
{
}

QVariant Configuration::item(const QString &name, const QVariant &defaultValue)
{
}
