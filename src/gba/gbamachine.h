#ifndef GBAMACHINE_H
#define GBAMACHINE_H

#if defined(__cplusplus)
class GbaCpu;
class GbaMemory;
class GbaSound;
class GbaVideo;
class GbaPad;
class GbaMachine;
#include <imachine.h>
#include <QImage>
#include <QThread>
#include <QSemaphore>
#include "gbacommon.h"

class GbaThread : public QThread {
	Q_OBJECT
protected:
	void run();
};

class GbaMachine : public IMachine {
	Q_OBJECT
public:
	explicit GbaMachine(QObject *parent = 0);
	~GbaMachine();
	void reset();

	QString setDisk(const QString &path);
	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	int audioPendingSize() const;
	int fillAudioBuffer(char *stream, int streamSize);
	void setPadKey(PadKey key, bool state);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	volatile bool m_quit;
	QSemaphore m_prodSem;
	QSemaphore m_consSem;
protected:
	void setAudioEnabled(bool on);
	void setAudioSampleRate(int sampleRate);
private:
	void loadBios();

	GbaCpu *m_cpu;
	GbaMemory *m_memory;
	GbaSound *m_sound;
	GbaVideo *m_video;
	GbaPad *m_pad;
	QString m_biosError;
	QImage m_frame;
};
#endif

typedef enum
{
  TIMER_INACTIVE,
  TIMER_PRESCALE,
  TIMER_CASCADE
} timer_status_type;

typedef enum
{
  TIMER_NO_IRQ,
  TIMER_TRIGGER_IRQ
} timer_irq_type;


typedef enum
{
  TIMER_DS_CHANNEL_NONE,
  TIMER_DS_CHANNEL_A,
  TIMER_DS_CHANNEL_B,
  TIMER_DS_CHANNEL_BOTH
} timer_ds_channel_type;

typedef struct
{
  s32 count;
  u32 reload;
  u32 prescale;
  u32 stop_cpu_ticks;
  fixed16_16 frequency_step;
  u32 direct_sound_channels;
  u32 irq;
  u32 status;
} timer_type;

#if defined(__cplusplus)
extern "C" {
#endif

extern u32 cpu_ticks;
extern u32 execute_cycles;
extern u32 global_cycles_per_instruction;
extern u32 skip_next_frame;

extern timer_type timer[4];
static u32 prescale_table[] = { 0, 6, 8, 10 };

extern u32 cycle_memory_access;
extern u32 cycle_pc_relative_access;
extern u32 cycle_sp_relative_access;
extern u32 cycle_block_memory_access;
extern u32 cycle_block_memory_sp_access;
extern u32 cycle_block_memory_words;
extern u32 cycle_dma16_words;
extern u32 cycle_dma32_words;
extern u32 flush_ram_count;

extern u64 base_timestamp;

u32 update_gba();
void reset_gba();
void game_name_ext(u8 *src, u8 *buffer, u8 *extension);

#define count_timer(timer_number)                                             \
  timer[timer_number].reload = 0x10000 - value;                               \
  if(timer_number < 2)                                                        \
  {                                                                           \
	u32 timer_reload =                                                        \
	 timer[timer_number].reload << timer[timer_number].prescale;              \
	sound_update_frequency_step(timer_number);                                \
  }                                                                           \

#define adjust_sound_buffer(timer_number, channel)                            \
  if(timer[timer_number].direct_sound_channels & (0x01 << channel))           \
  {                                                                           \
	direct_sound_channel[channel].buffer_index =                              \
	 (direct_sound_channel[channel].buffer_index + buffer_adjust) %           \
	 SOUND_BUFFER_SIZE;                                                             \
  }                                                                           \

#define trigger_timer(timer_number)                                           \
  if(value & 0x80)                                                            \
  {                                                                           \
	if(timer[timer_number].status == TIMER_INACTIVE)                          \
	{                                                                         \
	  u32 prescale = prescale_table[value & 0x03];                            \
	  u32 timer_reload = timer[timer_number].reload;                          \
																			  \
	  if((value >> 2) & 0x01)                                                 \
		timer[timer_number].status = TIMER_CASCADE;                           \
	  else                                                                    \
		timer[timer_number].status = TIMER_PRESCALE;                          \
																			  \
	  timer[timer_number].prescale = prescale;                                \
	  timer[timer_number].irq = (value >> 6) & 0x01;                          \
																			  \
	  address16(io_registers, 0x100 + (timer_number * 4)) =                   \
	   -timer_reload;                                                         \
																			  \
	  timer_reload <<= prescale;                                              \
	  timer[timer_number].count = timer_reload;                               \
																			  \
	  if(timer_reload < execute_cycles)                                       \
		execute_cycles = timer_reload;                                        \
																			  \
	  if(timer_number < 2)                                                    \
	  {                                                                       \
		u32 buffer_adjust =                                                   \
		 (u32)(((float)(cpu_ticks - timer[timer_number].stop_cpu_ticks) *     \
		 sound_frequency) / 16777216.0) * 2;                                  \
																			  \
		sound_update_frequency_step(timer_number);                            \
		adjust_sound_buffer(timer_number, 0);                                 \
		adjust_sound_buffer(timer_number, 1);                                 \
	  }                                                                       \
	}                                                                         \
  }                                                                           \
  else                                                                        \
  {                                                                           \
	if(timer[timer_number].status != TIMER_INACTIVE)                          \
	{                                                                         \
	  timer[timer_number].status = TIMER_INACTIVE;                            \
	  timer[timer_number].stop_cpu_ticks = cpu_ticks;                         \
	}                                                                         \
  }                                                                           \
  address16(io_registers, 0x102 + (timer_number * 4)) = value;                \

void change_ext(u8 *src, u8 *buffer, u8 *extension);

#if defined(__cplusplus)
}
#endif

#endif // GBAMACHINE_H
