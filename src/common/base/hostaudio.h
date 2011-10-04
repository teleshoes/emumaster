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

#ifndef HOSTAUDIO_H
#define HOSTAUDIO_H

class IMachine;
#include <QObject>
#include <pulse/pulseaudio.h>

class HostAudio : public QObject {
    Q_OBJECT
public:
	explicit HostAudio(IMachine *machine);
	~HostAudio();

	void open();
	void close();

	void sendFrame();

	pa_threaded_mainloop *mainloop() const;
private:
	void waitForStreamReady();

	pa_threaded_mainloop *m_mainloop;
	pa_context *m_context;
	pa_mainloop_api *m_api;
	pa_stream *m_stream;

	IMachine *m_machine;
};

inline pa_threaded_mainloop *HostAudio::mainloop() const
{ return m_mainloop; }

#endif // HOSTAUDIO_H
