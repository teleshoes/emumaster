#ifndef NESSYNC_P_H
#define NESSYNC_P_H

#include "sync.h"
#include "cpubase.h"
#include <memutils.h>
#include <macroassembler.h>
#include <QThread>

using namespace Arm;

class NesSyncData
{
public:
	void fillWithFuctions();

	u64 cpuCycleCounter;
	u64 baseCycleCounter;

	u32 nextPc; // next pc(ARM) in sync loop
	volatile NesCpuBase::Event cpuEvent;
	volatile u32 drawEnabled;

	 int (*begin)();
	void (*end)();
	void (*ppuProcessFrameStart)();
	void (*ppuSetVBlank)(bool on);
	 int (*ppuNextScanline)();
	void (*ppuProcessScanlineStart)();
	void (*ppuProcessScanlineNext)();
	void (*mapperVerticalSync)();
	void (*mapperHorizontalSync)();
	void (*updateZapper)();

	u32 returnFromPpuProcessing;
	bool (*ppuCheckSprite0)();
	void (*ppuDummyScanline)();

	bool (*ppuDrawPrologue)();
	void (*ppuDrawEpilogue)();
	void (*ppuDrawBackgroundTile)(int i);
	void (*ppuDrawBackgroundLine)();
};

class NesSyncCompiler
{
public:
	NesSync *recompile();

	void mCallCFunction(int offsetInData);

	void mEntryPoint();
	void mLeaveToCpu();

	void mBegin();
	void mEnd();

	void mClock(int ppuCycles);
	void mClockLine();

	void mPpuProcessScanlineFunction();

	void mEmulateFrame();
	void mEmulateVisibleScanline(bool process);
	void mEmulateHiddenScanline(bool zapperHere);

	void mPpuProcessFrameStart();
	void mPpuSetVBlank(bool on);
	void mPpuNextScanline();
	void mPpuProcessScanlineStart();
	void mPpuProcessScanlineNext();

	void mMapperVerticalSync();
	void mMapperHorizontalSync();
	void mUpdateZapper();
private:
	void initRenderType();

	MacroAssembler *m_masm;

	Label m_nesSyncLabel;
	Label m_processScanlineLabel;

	bool m_renderTile;
	bool m_renderPost;
	bool m_renderPre;
	bool m_renderAll;

	Register m_dataBase;
	Register m_additionalCpuCycles;
	RegList m_regList;
};

class NesSyncCpuRunner : public QThread
{
public:
	NesSyncCpuRunner(NesSync *nesSync);
protected:
	void run();
private:
	NesSync *m_nesSync;
};

#define __ m_masm->

#endif // NESSYNC_P_H
