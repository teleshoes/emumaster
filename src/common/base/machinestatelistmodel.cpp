#include "machinestatelistmodel.h"
#include "imachine.h"
#include <QDataStream>
#include <QDateTime>
#include <QImage>

MachineStateListModel::MachineStateListModel(IMachine *machine, const QString &diskName) :
	m_machine(machine),
	m_screenShotUpdateCounter(0) {

	QHash<int, QByteArray> roles;
	roles.insert(NameRole, "title");
	roles.insert(ScreenShotUpdate, "screenShotUpdate");
	roles.insert(DateTimeRole, "saveDateTime");
	setRoleNames(roles);

	m_dir = QDir(QString("%1/state").arg(IMachine::userDataDirPath()));

	QString subDirName = QString("%1_%2").arg(m_machine->name()).arg(diskName);
	m_dir.mkdir(subDirName);
	m_dir.cd(subDirName);

	m_list = m_dir.entryInfoList(QDir::Files, QDir::Time);
	m_maxSaveIndex = 0;
	foreach (QFileInfo info, m_list) {
		int i = info.fileName().toInt();
		m_maxSaveIndex = qMax(i, m_maxSaveIndex);
	}
}

QVariant MachineStateListModel::data(const QModelIndex &index, int role) const {
	if (role == NameRole) {
		return m_list.at(index.row()).fileName();
	} else if (role == ScreenShotUpdate) {
		return m_screenShotUpdateCounter;
	} else if (role == DateTimeRole) {
		return m_list.at(index.row()).lastModified();
	}
	return QVariant();
}

int MachineStateListModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent)
	return m_list.size();
}

int MachineStateListModel::count() const
{ return m_list.size(); }

QString MachineStateListModel::get(int i) const
{ return m_list.at(i).fileName(); }

QImage MachineStateListModel::screenShot(int i) const {
	QFile file(m_dir.filePath(QString::number(i)));
	if (!file.open(QIODevice::ReadOnly))
		return QImage();
	QDataStream s(&file);
	s.setByteOrder(QDataStream::LittleEndian);
	s.setFloatingPointPrecision(QDataStream::SinglePrecision);
	QImage screenShot;
	s >> screenShot;
	return screenShot;
}

bool MachineStateListModel::saveState(int i) {
	bool newState = false;
	if (i == NewSlot) {
		i = ++m_maxSaveIndex;
		newState = true;
	} else if (i == AutoSlot) {
		newState = (indexOf(-2) < 0);
	}	
	QByteArray data;
	data.reserve(10*1024*1024);
	QDataStream s(&data, QIODevice::WriteOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s.setFloatingPointPrecision(QDataStream::SinglePrecision);
	if (!m_machine->save(s))
		return false;

	QString name = QString::number(i);
	QFile file(m_dir.filePath(name));
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	s.setDevice(&file);
	s << m_machine->frame();
	QByteArray compressed = qCompress(data);
	bool ok = (file.write(compressed) == compressed.size());
	file.close();
	if (!ok) {
		file.remove();
		return false;
	}
	m_screenShotUpdateCounter++;
	if (newState) {
		beginInsertRows(QModelIndex(), 0, 0);
		m_list.prepend(QFileInfo(m_dir.filePath(name)));
		endInsertRows();
		emit countChanged();
	} else {
		int x = indexOf(i);
		m_list[x] = QFileInfo(m_list.at(x).filePath());
		emit dataChanged(index(x), index(x));
	}
	return true;
}

bool MachineStateListModel::loadState(int i) {
	if (i == AutoSlot) {
		if (m_list.size() <= 0)
			return false;
		i = m_list.at(0).fileName().toInt();
	}
	QString name = QString::number(i);
	QFile file(m_dir.filePath(name));
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QDataStream sOmit(&file);
	sOmit.setByteOrder(QDataStream::LittleEndian);
	sOmit.setFloatingPointPrecision(QDataStream::SinglePrecision);
	QImage omitFrame;
	sOmit >> omitFrame;

	QByteArray compressed = file.readAll();
	QByteArray data = qUncompress(compressed);
	file.close();
	compressed.clear();

	QDataStream s(&data, QIODevice::ReadOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s.setFloatingPointPrecision(QDataStream::SinglePrecision);
	bool ok = m_machine->load(s);
	if (!ok)
		m_machine->reset();
	return ok;
}

void MachineStateListModel::removeState(int i) {
	int x = indexOf(i);
	if (x >= 0) {
		beginRemoveRows(QModelIndex(), x, x);
		m_dir.remove(QString::number(i));
		m_list.removeAt(x);
		endRemoveRows();
	}
}

int MachineStateListModel::indexOf(int i) const {
	for (int x = 0; x < m_list.size(); x++) {
		if (m_list.at(x).fileName().toInt() == i)
			return x;
	}
	return -1;
}
