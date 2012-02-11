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

#ifndef NESMACHINE_H
#define NESMACHINE_H

#include <imachine.h>
class NesPpu;

#define NES_PPU_NTSC_CLK	21477270.0
#define NES_PPU_PAL_CLK		26601712.0

#define NES_CPU_NTSC_CLK	NES_PPU_NTSC_CLK/12.0
#define NES_CPU_PAL_CLK		NES_PPU_PAL_CLK/15.0

#define NES_NTSC_FRAMERATE	60.098
#define NES_PAL_FRAMERATE	50.007

#define NES_NTSC_SCANLINE_CLOCKS	1364
#define NES_PAL_SCANLINE_CLOCKS		1705

enum SystemType { NES_NTSC, NES_PAL };

class NesMachine : public IMachine {
	Q_OBJECT
	Q_PROPERTY(NesPpu *ppu READ ppu CONSTANT)
public:
	NesMachine();
	QString init(const QString &diskPath);
	void shutdown();
	void reset();

	NesPpu *ppu() const;

	QString setDisk(const QString &path);

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
	void setPadKeys(int pad, int keys);

	void setupTvEncodingSystem(const QString &path);
	bool slCheckTvEncodingSystem() const;
};

extern NesMachine nesMachine;
extern SystemType nesSystemType;

#endif // NESMACHINE_H
