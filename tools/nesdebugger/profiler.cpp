#include "profiler.h"

NesProfiler::NesProfiler(const QList<ProfilerItem> &items, QObject *parent) :
	QAbstractListModel(parent),
	m_items(items)
{
	QHash<int, QByteArray> roles;
	roles.insert(PcRole, "pc");
	roles.insert(CountRole, "cnt");
	setRoleNames(roles);
}

int NesProfiler::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return m_items.size();
}

void NesProfiler::reset()
{
	QAbstractListModel::reset();
}

QVariant NesProfiler::data(const QModelIndex &index, int role) const
{
	if (role == PcRole) {
		return QString("%1").arg(m_items.at(index.row()).first,
								 4, 16, QLatin1Char('0'));
	} else if (role == CountRole) {
		return m_items.at(index.row()).second;
	}
	return QVariant();
}
