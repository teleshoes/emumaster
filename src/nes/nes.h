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

#ifndef NESEMU_H
#define NESEMU_H

#include <emu.h>
class NesPpu;

enum SystemType { NES_NTSC, NES_PAL };

class NesEmu : public Emu
{
	Q_OBJECT
	Q_PROPERTY(NesPpu *ppu READ ppu CONSTANT)
	Q_PROPERTY(QObject *cheats READ cheats CONSTANT)
public:
	enum RenderMethod {
		PostAllRender,
		PreAllRender,
		PostRender,
		PreRender,
		TileRender
	};

	NesEmu();
	QString init(const QString &diskPath);
	void shutdown();
	void reset();

	void resume();

	NesPpu *ppu() const;
	QObject *cheats() const;
	u64 cpuCycles() const;

	QString setDisk(const QString &path);

	RenderMethod renderMethod() const;
	void setRenderMethod(RenderMethod method);

	void clockCpu(u32 cycles);
	const QImage &frame() const;
	void emulateFrame(bool drawEnabled);
	int fillAudioBuffer(char *stream, int streamSize);
protected:
	void sl();
private:
	void emulateFrameNoTile(bool drawEnabled);
	void emulateVisibleScanlineNoTile();

	void emulateFrameTile(bool drawEnabled);
	void emulateVisibleScanlineTile();

	void updateZapper();

	void setupTvEncodingSystem(const QString &path);
	bool slCheckTvEncodingSystem() const;
};

extern NesEmu nesEmu;
extern SystemType nesSystemType;

#endif // NESEMU_H
