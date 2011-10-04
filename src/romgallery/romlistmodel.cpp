/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "romlistmodel.h"
#include "imachine.h"
#include <QFileInfo>
#include <QSettings>
#include <QUrl>
#include <QProcess>

RomListModel::RomListModel(QObject *parent) :
	QAbstractListModel(parent),
	m_screenShotUpdateCounter(0) {

	QHash<int, QByteArray> roles;
	roles.insert(TitleRole, "title");
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
		m_list.clear();
		endRemoveRows();
	}
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
			m_list.append(infoList.at(i).fileName());
	}
	if (!m_list.isEmpty()) {
		beginInsertRows(QModelIndex(), 0, m_list.size()-1);
		endInsertRows();
	}
}

QVariant RomListModel::data(const QModelIndex &index, int role) const {
	if (role == TitleRole) {
		return getDiskTitle(index.row());
	} else if (role == AlphabetRole) {
		QString title = getDiskTitle(index.row());
		if (title.isEmpty())
			return QVariant();
		return title.at(0).toUpper();
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

QString RomListModel::getDiskTitle(int i) const {
	if (i < 0 || i >= m_list.size())
		return QString();
	return QFileInfo(m_list.at(i)).completeBaseName();
}

QString RomListModel::getDiskFileName(int i) const {
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
	QString diskTitle = getDiskTitle(i);
	if (diskTitle.isEmpty())
		return -1;
	QString path = QString("%1/screenshot/%2_%3.jpg")
			.arg(IMachine::userDataDirPath())
			.arg(m_machineName)
			.arg(diskTitle);
	if (QFile::exists(path))
		return m_screenShotUpdateCounter;
	else
		return -1;
}

void RomListModel::trash(int i) {
	beginRemoveRows(QModelIndex(), i, i);
	QStringList args;
	args << "-R";
	args << m_dir.absolutePath() + "/" + m_list.at(i);
	QProcess::startDetached("rm", args);
	m_list.removeAt(i);
	endRemoveRows();
}

void RomListModel::receiveDatagram() {
	QByteArray ba(m_sock.pendingDatagramSize(), Qt::Uninitialized);
	m_sock.readDatagram(ba.data(), ba.size());
	QDataStream s(&ba, QIODevice::ReadOnly);
	QString diskFileName;
	s >> diskFileName;
	updateScreenShot(diskFileName);
}

void RomListModel::setDiskCover(int i, const QUrl &coverUrl) {
	QString diskTitle = getDiskTitle(i);
	if (diskTitle.isEmpty())
		return;
	QString path = QString("%1/screenshot/%2_%3.jpg")
			.arg(IMachine::userDataDirPath())
			.arg(m_machineName)
			.arg(diskTitle);
	QString coverPath = coverUrl.toLocalFile();
	if (!QFile::exists(coverPath))
		return;
	QFile::remove(path);
	QFile::copy(coverPath, path);
	m_screenShotUpdateCounter++;
	emit dataChanged(index(i), index(i));
}
