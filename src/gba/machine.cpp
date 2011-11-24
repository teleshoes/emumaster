#include "machineview.h"
#include <pathmanager.h>
#include <QFile>
#include <QApplication>
#include "common.h"
#include "cpu.h"
#include "mem.h"
#include "gpu.h"
#include "spu.h"
#include "machine.h"
#include "cheats.h"

timer_type timer[4];

u32 breakpoint_value = 0x7c5000;

u32 global_cycles_per_instruction = 1;

u32 cpu_ticks = 0;

u32 execute_cycles = 960;
s32 video_count = 960;
u32 ticks;

u32 arm_frame = 0;
u32 thumb_frame = 0;

u32 skip_next_frame = 0;

u32 cycle_memory_access = 0;
u32 cycle_pc_relative_access = 0;
u32 cycle_sp_relative_access = 0;
u32 cycle_block_memory_access = 0;
u32 cycle_block_memory_sp_access = 0;
u32 cycle_block_memory_words = 0;
u32 cycle_dma16_words = 0;
u32 cycle_dma32_words = 0;
u32 flush_ram_count = 0;
u32 gbc_update_count = 0;
u32 oam_update_count = 0;

extern "C" u16 *screen_pixels_ptr;
extern "C" void return_to_host(u32 *returnRegs);
static u32 return_to_host_regs[2];

static QImage gpuFrame;
static GbaThread gbaThread;

GbaMachine gbaMachine;

GbaMachine::GbaMachine() :
	IMachine("gba") {
}

QString GbaMachine::init() {
	gpuFrame = QImage(240, 160, QImage::Format_RGB16);
	setVideoSrcRect(gpuFrame.rect());
	setFrameRate(60);

	screen_pixels_ptr = (quint16 *)gpuFrame.bits();
	m_quit = false;
	return loadBios();
}

void GbaMachine::shutdown() {
	m_quit = true;
	emulateFrame(false);
	gbaThread.wait();
	gpuFrame = QImage();
}

void GbaMachine::reset() {
	skip_next_frame = 0;

	for (int i = 0; i < 4; i++) {
		dma[i].start_type = DMA_INACTIVE;
		dma[i].direct_sound_channel = DMA_NO_DIRECT_SOUND;
		timer[i].status = TIMER_INACTIVE;
		timer[i].reload = 0x10000;
		timer[i].stop_cpu_ticks = 0;
	}

	timer[0].direct_sound_channels = TIMER_DS_CHANNEL_BOTH;
	timer[1].direct_sound_channels = TIMER_DS_CHANNEL_NONE;

	cpu_ticks = 0;

	execute_cycles = 960;
	video_count = 960;

	init_memory();
	init_cpu();
	reset_sound();
	reg[CHANGED_PC_STATUS] = 1;

	flush_translation_cache_rom();
	flush_translation_cache_ram();
	flush_translation_cache_bios();
}

QString GbaMachine::loadBios() {
	QString path = PathManager::instance()->diskDirPath() + "/gba_bios.bin";
	QFile biosFile(path);

	bool loaded = false;
	bool supported = false;
	if (biosFile.open(QIODevice::ReadOnly)) {
		if (biosFile.read(reinterpret_cast<char *>(bios_rom), 0x4000) == 0x4000) {
			loaded = true;
			supported = (bios_rom[0] == 0x18);
		}
	}
	if (!loaded) {
		return
				"Sorry, but emulator requires a Gameboy Advance BIOS\n"
				"image to run correctly. Make sure to get an        \n"
				"authentic one, it'll be exactly 16384 bytes large  \n"
				"and should have the following md5sum value:        \n"
				"                                                   \n"
				"a860e8c0b6d573d191e4ec7db1b1e4f6                   \n"
				"                                                   \n"
				"When you do get it name it gba_bios.bin and put it \n"
				"in the \"emumaster/gba\" directory.                ";

	} else if (!supported) {
		return "You have an incorrect BIOS image.";
	}
	return QString();
}

QString GbaMachine::setDisk(const QString &path) {
	init_gamepak_buffer();
	if (!gbaMem.loadGamePack(path))
		return "Could not load ROM";
	reset();
	skip_next_frame = 1;

	gbaThread.start();
	m_consSem.acquire();
	return QString();
}

const QImage &GbaMachine::frame() const
{ return gpuFrame; }

void GbaMachine::emulateFrame(bool drawEnabled) {
	skip_next_frame = !drawEnabled;
	screen_pixels_ptr = (quint16 *)gpuFrame.bits();
	m_prodSem.release();
	m_consSem.acquire();
}

void GbaMachine::sync() {
	m_consSem.release();
	if (m_quit)
		return_to_host(return_to_host_regs);
	m_prodSem.acquire();
}

static const int keyMapping[10] = {
	IMachine::A_PadKey,
	IMachine::B_PadKey,
	IMachine::Select_PadKey,
	IMachine::Start_PadKey,
	IMachine::Right_PadKey,
	IMachine::Left_PadKey,
	IMachine::Up_PadKey,
	IMachine::Down_PadKey,
	IMachine::R_PadKey,
	IMachine::L_PadKey
};

void GbaMachine::setPadKeys(int pad, int keys) {
	if (pad)
		return;
	int gbaKeys = 0x3FF;
	for (int i = 0; i < 10; i++) {
		if (keys & keyMapping[i])
			gbaKeys &= ~(1 << i);
	}
	io_registers[REG_P1] = gbaKeys;
}

int GbaMachine::fillAudioBuffer(char *stream, int streamSize)
{ return gbaSpu.fillBuffer(stream, streamSize); }
void GbaMachine::setAudioEnabled(bool on)
{ gbaSpu.setEnabled(on); }

void GbaThread::run() {
	execute_arm_translate(execute_cycles, return_to_host_regs);
}

#define check_count(count_var) \
	if (count_var < execute_cycles) \
		execute_cycles = count_var;

#define check_timer(timer_number) \
	if (timer[timer_number].status == TIMER_PRESCALE) \
		check_count(timer[timer_number].count);

#define update_timer(timer_number) \
	if (timer[timer_number].status != TIMER_INACTIVE) { \
		if (timer[timer_number].status != TIMER_CASCADE) { \
			timer[timer_number].count -= execute_cycles; \
			io_registers[REG_TM##timer_number##D] = -(timer[timer_number].count >> timer[timer_number].prescale); \
		} \
		if (timer[timer_number].count <= 0) { \
			if (timer[timer_number].irq == TIMER_TRIGGER_IRQ) \
				irq_raised |= IRQ_TIMER##timer_number; \
			if ((timer_number != 3) && (timer[timer_number + 1].status == TIMER_CASCADE)) { \
				timer[timer_number + 1].count--; \
				io_registers[REG_TM0D + (timer_number + 1) * 2] = -(timer[timer_number + 1].count); \
			} \
			if (timer_number < 2) { \
				if (timer[timer_number].direct_sound_channels & 0x01) \
					sound_timer(timer[timer_number].frequency_step, 0); \
				if (timer[timer_number].direct_sound_channels & 0x02) \
					sound_timer(timer[timer_number].frequency_step, 1); \
			} \
			timer[timer_number].count += (timer[timer_number].reload << timer[timer_number].prescale); \
		} \
	}

u32 update_gba() {
	int irq_raised = IRQ_NONE;
	do {
		bool do_sync = false;
		cpu_ticks += execute_cycles;
		reg[CHANGED_PC_STATUS] = 0;
		if (gbc_sound_update) {
			gbc_update_count++;
			update_gbc_sound(cpu_ticks);
			gbc_sound_update = 0;
		}
		update_timer(0);
		update_timer(1);
		update_timer(2);
		update_timer(3);

		video_count -= execute_cycles;

		if (video_count <= 0) {
			u32 vcount = io_registers[REG_VCOUNT];
			u32 dispstat = io_registers[REG_DISPSTAT];

			if ((dispstat & 0x02) == 0) {
				// Transition from hrefresh to hblank
				video_count += (272);
				dispstat |= 0x02;

				if ((dispstat & 0x01) == 0) {
					if (oam_update)
						oam_update_count++;
					update_scanline();
					// If in visible area also fire HDMA
					for (int i = 0; i < 4; i++) {
						if (dma[i].start_type == DMA_START_HBLANK)
							dma_transfer(dma + i);
					}
				}
				if (dispstat & 0x10)
					irq_raised |= IRQ_HBLANK;
			} else {
				// Transition from hblank to next line
				video_count += 960;
				dispstat &= ~0x02;
				vcount++;
				if (vcount == 160) {
					// Transition from vrefresh to vblank
					dispstat |= 0x01;
					if (dispstat & 0x8)
						irq_raised |= IRQ_VBLANK;

					affine_reference_x[0] = (s32)(address32(io_registers, 0x28) << 4) >> 4;
					affine_reference_y[0] = (s32)(address32(io_registers, 0x2C) << 4) >> 4;
					affine_reference_x[1] = (s32)(address32(io_registers, 0x38) << 4) >> 4;
					affine_reference_y[1] = (s32)(address32(io_registers, 0x3C) << 4) >> 4;

					for (int i = 0; i < 4; i++) {
						if (dma[i].start_type == DMA_START_VBLANK)
							dma_transfer(dma + i);
					}
				} else if (vcount == 228) {
					// Transition from vblank to next screen
					dispstat &= ~0x01;
					update_gbc_sound(cpu_ticks);
					process_cheats();
					vcount = 0;
					do_sync = true;
				}
				if (vcount == (dispstat >> 8)) {
					// vcount trigger
					dispstat |= 0x04;
					if (dispstat & 0x20)
						irq_raised |= IRQ_VCOUNT;
				} else {
					dispstat &= ~0x04;
				}
				io_registers[REG_VCOUNT] = vcount;
			}
			io_registers[REG_DISPSTAT] = dispstat;
		}
		if (irq_raised)
			raise_interrupt(static_cast<irq_type>(irq_raised));
		execute_cycles = video_count;

		check_timer(0);
		check_timer(1);
		check_timer(2);
		check_timer(3);
		if (do_sync)
			gbaMachine.sync();
	} while (reg[CPU_HALT_STATE] != CPU_ACTIVE);
	return execute_cycles;
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(&gbaMachine, argv[1]);
	return app.exec();
}

void GbaMachine::sl() {
	emsl.begin("machine");
	emsl.var("cpu_ticks", cpu_ticks);
	emsl.var("execute_cycles", execute_cycles);
	emsl.var("video_count", video_count);
	emsl.array("timer", timer, sizeof(timer));
	emsl.end();

	gbaCpu.sl();
	gbaSpu.sl();
	gbaGpu.sl();
	gbaMem.sl();
}
