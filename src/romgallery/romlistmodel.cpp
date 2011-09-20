#include "romlistmodel.h"
#include "imachine.h"
#include <QFileInfo>
#include <QSettings>
#include <QUrl>

RomListModel::RomListModel(QObject *parent) :
	QAbstractListModel(parent),
	m_screenShotUpdateCounter(0) {

	QHash<int, QByteArray> roles;
	roles.insert(NameRole, "title");
	roles.insert(AlphabetRole, "alphabet");
	roles.insert(ScreenShotUpdate, "screenShotUpdate");
	setRoleNames(roles);

	QSettings s("elemental", "emumaster");
	s.beginGroup("romgallery");
	m_machineNameLastUsed = s.value("machineNameLastUsed", "nes").toString();
	s.endGroup();

	m_sock.bind(QHostAddress::LocalHost, 5798);
	QObject::connect(&m_sock, SIGNAL(readyRead()), SLOT(receiveDatagram()));
}

RomListModel::~RomListModel() {
	QSettings s("elemental", "emumaster");
	s.beginGroup("romgallery");
	s.setValue("machineNameLastUsed", m_machineName);
	s.endGroup();
}

void RomListModel::setMachineName(const QString &name) {
	if (!m_list.isEmpty()) {
		beginRemoveRows(QModelIndex(), 0, m_list.size()-1);
		endRemoveRows();
	}
	m_list.clear();
	m_machineName = name;
	emit machineNameChanged();
	m_dir = QDir(IMachine::diskDirPath(m_machineName));

	QFileInfoList infoList = m_dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name|QDir::IgnoreCase);

	QStringList excluded;
	if (name == "gba") {
		excluded << "gba_bios.bin";
	} else if (name == "psx") {
		excluded << "mcd001.mcr";
		excluded << "mcd002.mcr";
	}
	QRegExp psxBiosRx("scph*.bin", Qt::CaseSensitive, QRegExp::Wildcard);
	for (int i = 0; i < infoList.size(); i++) {
		if (psxBiosRx.exactMatch(infoList.at(i).fileName()))
			continue;
		if (!excluded.contains(infoList.at(i).fileName()))
			m_list.append(infoList.at(i).completeBaseName());
	}
	if (!m_list.isEmpty()) {
		beginInsertRows(QModelIndex(), 0, m_list.size()-1);
		endInsertRows();
	}
}

QVariant RomListModel::data(const QModelIndex &index, int role) const {
	if (role == NameRole) {
		return m_list.at(index.row());
	} else if (role == AlphabetRole) {
		return m_list.at(index.row()).at(0).toUpper();
	} else if (role == ScreenShotUpdate) {
		return getScreenShotUpdate(index.row());
	}
	return QVariant();
}

int RomListModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent)
	return m_list.size();
}

int RomListModel::count() const
{ return m_list.size(); }

QString RomListModel::getAlphabet(int i) const {
	if (i < 0 || i >= m_list.size())
		return QString();
	return m_list.at(i).at(0).toUpper();
}

QString RomListModel::get(int i) const {
	if (i < 0 || i >= m_list.size())
		return QString();
	return m_list.at(i);
}

void RomListModel::updateScreenShot(const QString &name) {
	int i = m_list.indexOf(name);
	if (i < 0)
		return;
	m_screenShotUpdateCounter++;
	emit dataChanged(index(i), index(i));
}

int RomListModel::getScreenShotUpdate(int i) const {
	QString path = QString("%1/screenshot/%2_%3.jpg")
			.arg(IMachine::userDataDirPath())
			.arg(m_machineName)
			.arg(m_list.at(i));
	if (QFile::exists(path))
		return m_screenShotUpdateCounter;
	else
		return -1;
}

void RomListModel::trash(int i) {
	QStringList nf;
	nf << m_list.at(i);
	QFileInfoList list = m_dir.entryInfoList(nf, QDir::Dirs);
	if (!list.isEmpty()) {
		QString fn = list.at(0).fileName();
		m_dir.cd(fn);
		nf = QStringList() << "*";
		list = m_dir.entryInfoList(nf);
		foreach (QFileInfo info, list)
			m_dir.remove(info.fileName());
		m_dir.cdUp();
		m_dir.rmdir(fn);
	} else {
		nf = QStringList() << (m_list.at(i) + ".*");
		list = m_dir.entryInfoList(nf);
		foreach (QFileInfo info, list)
			m_dir.remove(info.fileName());
	}
	setMachineName(m_machineName);
}

void RomListModel::receiveDatagram() {
	QByteArray ba(m_sock.pendingDatagramSize(), Qt::Uninitialized);
	m_sock.readDatagram(ba.data(), ba.size());
	QDataStream s(&ba, QIODevice::ReadOnly);
	QString name;
	s >> name;
	updateScreenShot(name);
}

void RomListModel::setDiskCover(int i, const QUrl &coverUrl) {
	if (i < 0)
		return;
	QString path = QString("%1/screenshot/%2_%3.jpg")
			.arg(IMachine::userDataDirPath())
			.arg(m_machineName)
			.arg(m_list.at(i));
	QString coverPath = coverUrl.toLocalFile();
	if (!QFile::exists(coverPath))
		return;
	QFile::remove(path);
	QFile::copy(coverPath, path);
	m_screenShotUpdateCounter++;
	emit dataChanged(index(i), index(i));
}
