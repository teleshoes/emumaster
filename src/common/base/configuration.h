#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QVariant>

class Configuration : public QObject {
    Q_OBJECT
public:
    explicit Configuration(QObject *parent = 0);
	QVariant item(const QString &name,
				  const QVariant &defaultValue = QVariant());
	void setItem(const QString &name, const QVariant &value);
	void setItemHard(const QString &name, const QVariant &value);
};

#endif // CONFIGURATION_H
