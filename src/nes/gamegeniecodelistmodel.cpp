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

#include "gamegeniecodelistmodel.h"
#include "machine.h"
#include "mapper.h"
#include "disk.h"
#include "gamegeniecode.h"
#include <pathmanager.h>
#include <QFile>
#include <QFileInfo>

GameGenieCodeListModel::GameGenieCodeListModel() {
	QHash<int, QByteArray> roles;
	roles.insert(CodeRole, "code");
	roles.insert(DescriptionRole, "description");
	roles.insert(EnableRole, "isEnabled");
	setRoleNames(roles);
}

void GameGenieCodeListModel::sl() {
	emsl.begin("cheats");
	emsl.var("codes", m_codes);
	emsl.var("descriptions", m_descriptions);
	emsl.var("enable", m_enable);
	emsl.end();
	if (!emsl.save)
		nesMapper->setGameGenieCodeList(enabledList());
}

QList<GameGenieCode> GameGenieCodeListModel::enabledList() const {
	QList<GameGenieCode> result;
	for (int i = 0; i < m_codes.size(); i++) {
		if (m_enable.at(i)) {
			GameGenieCode gcc;
			gcc.parse(m_codes.at(i));
			result.append(gcc);
		}
	}
	return result;
}

void GameGenieCodeListModel::setEnabled(int i, bool on) {
	if (i >= 0 && i < m_enable.size() && m_enable.at(i) != on) {
		m_enable[i] = on;
		emit dataChanged(index(i), index(i));
		nesMapper->setGameGenieCodeList(enabledList());
	}
}

int GameGenieCodeListModel::count() const
{ return m_codes.size(); }

int GameGenieCodeListModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent)
	return m_codes.size();
}

QVariant GameGenieCodeListModel::data(const QModelIndex &index, int role) const {
	int i = index.row();
	if (i < 0 || i >= m_codes.size())
		return QVariant();
	if (role == CodeRole)
		return m_codes.at(i);
	else if (role == DescriptionRole)
		return m_descriptions.at(i);
	else if (role == EnableRole)
		return m_enable.at(i);
	return QVariant();
}

void GameGenieCodeListModel::addNew(const QString &code, const QString &description) {
	QString codeUpper = code.toUpper();
	if (m_codes.contains(codeUpper))
		return;

	beginInsertRows(QModelIndex(), m_codes.size(), m_codes.size());
	m_codes.append(codeUpper);
	m_descriptions.append(description);
	m_enable.append(true);
	endInsertRows();

	nesMapper->setGameGenieCodeList(enabledList());
	emit modified();
}

void GameGenieCodeListModel::removeAt(int i) {
	if (i < 0 || i >= m_codes.size())
		return;

	beginRemoveRows(QModelIndex(), i, i);
	m_codes.removeAt(i);
	m_descriptions.removeAt(i);
	m_enable.removeAt(i);
	endRemoveRows();

	nesMapper->setGameGenieCodeList(enabledList());
	emit modified();
}

bool GameGenieCodeListModel::isCodeValid(const QString &s) {
	GameGenieCode code;
	return code.parse(s.toUpper());
}
