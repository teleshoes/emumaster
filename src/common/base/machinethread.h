/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef MACHINETHREAD_H
#define MACHINETHREAD_H

class IMachine;
class HostAudio;
class MachineStateListModel;
#include <QThread>

class MachineThread : public QThread {
    Q_OBJECT
public:
	explicit MachineThread(IMachine *machine);
	~MachineThread();

	int frameSkip() const;
	void setFrameSkip(int n);
	void setLoadSlot(int i);
	void setStateListModel(MachineStateListModel *stateListModel);
public slots:
	void resume();
	void pause();
signals:
	void frameGenerated(bool videoOn);
protected:
	void run();
private:
	IMachine *m_machine;

	volatile bool m_running;
	volatile bool m_inFrameGenerated;

	int m_frameSkip;

	bool m_firstRun;
	int m_loadSlot;
	MachineStateListModel *m_stateListModel;

	friend class HostVideo;
};

inline int MachineThread::frameSkip() const
{ return m_frameSkip; }

#endif // MACHINETHREAD_H
