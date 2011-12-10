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

#include "machine.h"
#include <machineview.h>
#include <pathmanager.h>
#include <QImage>
#include <QSemaphore>
#include <QApplication>

#include "events.h"
#include "mem.h"
#include "spu.h"
#include "mem.h"
#include "custom.h"
#include "cpu.h"
#include "disk.h"
#include "drawing.h"
#include "cia.h"

static volatile bool amigaGoingShutdown = false;
static QImage amigaFrame;

AmigaMachine amigaMachine;
AmigaThread amigaThread;

QSemaphore frameConsSem;
QSemaphore frameProdSem;

AmigaMachine::AmigaMachine(QObject *parent) :
	IMachine("amiga", parent) {
}

void AmigaMachine::reset() {
	amigaCpuSetSpcFlag(SpcFlagBrk);
}

QString AmigaMachine::init(const QString &diskPath) {
	setFrameRate(50);
	// TODO ntsc/pal configurable
	setVideoSrcRect(QRect(0, MINFIRSTLINE_PAL, 320, MAXVPOS_PAL-MINFIRSTLINE_PAL-VBLANK_ENDLINE_PAL));
	amigaFrame = QImage(320, MAXVPOS, QImage::Format_RGB16);

	amigaMemChipSize = 0x00100000; // TODO configurable chip size
	amigaMemInit();
	if (!amigaLoadKickstart(PathManager::instance()->diskDirPath()+"/kick13.rom"))
		return tr("Could not load kickstart");

	if (!amigaDrives[0].insertDisk(diskPath))
		return tr("Error inserting disk.");

	amigaThread.start();
	frameConsSem.acquire();
	return QString();
}

void AmigaMachine::shutdown() {
	amigaCpuSetSpcFlag(SpcFlagBrk);
	amigaCpuReleaseTimeslice();
	amigaGoingShutdown = true;
	frameProdSem.release();
	amigaThread.wait(2000);
	amigaFrame = QImage();
}

void AmigaMachine::emulateFrame(bool drawEnabled) {
	amigaDrawEnabled = drawEnabled;
	amigaFrame.bits();
	frameProdSem.release();
	frameConsSem.acquire();
	updateInput();
}

const QImage &AmigaMachine::frame() const
{ return amigaFrame; }
int AmigaMachine::fillAudioBuffer(char *stream, int streamSize)
{ return amigaSpuFillAudioBuffer(stream, streamSize); }

void AmigaMachine::setJoy(int joy, int buttons) {
	amigaInputPortButtons[joy] |= buttons >> 4;

	if (buttons)
		m_inputPortToggle[joy] = false;
	if (!m_inputPortToggle[joy])
		amigaInputPortDir[joy] = 0;
	if (buttons & 0xF) {
		if (buttons & PadKey_Up)
			amigaInputPortDir[joy] |= (1 << 8);
		if (buttons & PadKey_Down)
			amigaInputPortDir[joy] |= (1 << 0);
		if (buttons & PadKey_Left)
			amigaInputPortDir[joy] ^= (3 << 8);
		if (buttons & PadKey_Right)
			amigaInputPortDir[joy] ^= (3 << 0);
	}
}

void AmigaMachine::updateInput() {
	amigaInputPortButtons[0] = 0;
	amigaInputPortButtons[1] = 0;

	setJoy(0, padOffset(m_inputData, 0)[0]);
	setJoy(1, padOffset(m_inputData, 1)[0]);

	int *mouse0Data = mouseOffset(m_inputData, 0);
	setMouse(0, mouse0Data[0], mouse0Data[1], mouse0Data[2]);
	int *mouse1Data = mouseOffset(m_inputData, 1);
	setMouse(1, mouse1Data[0], mouse1Data[1], mouse1Data[2]);

	int key;
	do {
		key = keybDequeue(m_inputData);
		if (key)
			amigaRecordKey(key & ~(1<<31), key & (1<<31));
	} while (key != 0);
}

void AmigaMachine::setMouse(int mouse, int buttons, int dx, int dy) {
	dx = qBound(-127, dx/2, 127);
	dy = qBound(-127, dy/2, 127);
	if (!(dx | dy | buttons))
		return;

	m_inputPortToggle[mouse] = true;
	amigaInputPortButtons[mouse] |= buttons;
	u16 oldMouseRel = amigaInputPortDir[mouse];
	u8 mouseX = (oldMouseRel >> 0) & 0xFF;
	u8 mouseY = (oldMouseRel >> 8) & 0xFF;
	mouseX += dx;
	mouseY += dy;
	amigaInputPortDir[mouse] = (mouseY << 8) | mouseX;
}

void AmigaThread::run() {
	amigaCustomInit();
	amigaDiskReset();
	amigaCpuInit();
	amigaDrawInit((char *)amigaFrame.bits(), amigaFrame.bytesPerLine());
	amigaEventsInit();
	amigaCustomReset();
	amigaSpuDefaultEvtime();

	while (!amigaGoingShutdown) {
		amigaCpuReset();
		amigaCpuRun();
	}

	amigaCustomShutdown();
	amigaMemShutdown();
}

void AmigaMachine::sl() {
	amigaCpuSl();
	amigaMemSl();
	amigaDiskSl();
	amigaCustomSl();
	amigaSpuSl();
	amigaCiaSl();
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(&amigaMachine, argv[1]);
	return app.exec();
}
