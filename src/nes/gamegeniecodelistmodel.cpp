#include "gamegeniecodelistmodel.h"
#include "nesmachine.h"
#include "nesdisk.h"
#include "gamegeniecode.h"
#include <QFile>

GameGenieCodeListModel::GameGenieCodeListModel(NesMachine *machine) :
	m_machine(machine) {
	QHash<int, QByteArray> roles;
	roles.insert(CodeRole, "code");
	roles.insert(DescriptionRole, "description");
	roles.insert(EnableRole, "isEnabled");
	setRoleNames(roles);

	m_file.setFileName(filePath());
	load();
}

GameGenieCodeListModel::~GameGenieCodeListModel() {
	save();
}

void GameGenieCodeListModel::save() {
	if (m_codes.isEmpty()) {
		m_file.remove();
		return;
	}
	if (!m_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return;
	QDataStream s(&m_file);
	s << m_codes;
	s << m_descriptions;
	s << m_enable;
	m_file.close();
}

void GameGenieCodeListModel::load() {
	if (!m_file.open(QIODevice::ReadOnly))
		return;
	QDataStream s(&m_file);
	s >> m_codes;
	s >> m_descriptions;
	s >> m_enable;
	m_file.close();

	m_machine->mapper()->setGameGenieCodeList(enabledList());
}

QString GameGenieCodeListModel::filePath() {
	return QString("%1/cheat/%2_%3")
			.arg(IMachine::userDataDirPath())
			.arg(m_machine->name())
			.arg(m_machine->disk()->crc());
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
		m_machine->mapper()->setGameGenieCodeList(enabledList());
	}
}

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
	if (m_codes.contains(code))
		return;
	beginInsertRows(QModelIndex(), m_codes.size(), m_codes.size());
	m_codes.append(code);
	m_descriptions.append(description);
	m_enable.append(false);
	endInsertRows();
	m_machine->mapper()->setGameGenieCodeList(enabledList());
}

void GameGenieCodeListModel::removeAt(int i) {
	if (i < 0 || i >= m_codes.size())
		return;
	beginRemoveRows(QModelIndex(), i, i);
	m_codes.removeAt(i);
	m_descriptions.removeAt(i);
	m_enable.removeAt(i);
	endRemoveRows();
	m_machine->mapper()->setGameGenieCodeList(enabledList());
}

bool GameGenieCodeListModel::isCodeValid(const QString &s) {
	GameGenieCode code;
	return code.parse(s);
}
