#ifndef PROFILER_H
#define PROFILER_H

#include <base/emu.h>
#include <QAbstractListModel>

class ProfilerItem : public QPair<u16, int>
{
public:
	bool operator <(const ProfilerItem &i) const
	{
		if (second > i.second)
			return true;
		if (second == i.second && first < i.first)
			return true;
		return false;
	}
};

class NesProfiler : public QAbstractListModel
{
	Q_OBJECT
public:
	enum Role {
		PcRole = Qt::UserRole+1,
		CountRole
	};

	explicit NesProfiler(const QList<ProfilerItem> &items, QObject *parent = 0);
	int rowCount(const QModelIndex &parent) const;
	void reset();
	QVariant data(const QModelIndex &index, int role) const;
private:
	const QList<ProfilerItem> &m_items;
};

#endif // PROFILER_H
