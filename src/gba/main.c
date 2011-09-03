/* gameplaySP
 *
 * Copyright (C) 2006 Exophase <exophase@gmail.com>
 *
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

#include "common.h"

timer_type timer[4];

//debug_state current_debug_state = COUNTDOWN_BREAKPOINT;
//debug_state current_debug_state = PC_BREAKPOINT;
u32 breakpoint_value = 0x7c5000;
debug_state current_debug_state = RUN;
//debug_state current_debug_state = STEP_RUN;

//u32 breakpoint_value = 0;

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

#define check_count(count_var)                                                \
  if(count_var < execute_cycles)                                              \
    execute_cycles = count_var;                                               \

#define check_timer(timer_number)                                             \
  if(timer[timer_number].status == TIMER_PRESCALE)                            \
    check_count(timer[timer_number].count);                                   \

#define update_timer(timer_number)                                            \
  if(timer[timer_number].status != TIMER_INACTIVE)                            \
  {                                                                           \
    if(timer[timer_number].status != TIMER_CASCADE)                           \
    {                                                                         \
      timer[timer_number].count -= execute_cycles;                            \
      io_registers[REG_TM##timer_number##D] =                                 \
       -(timer[timer_number].count >> timer[timer_number].prescale);          \
    }                                                                         \
                                                                              \
    if(timer[timer_number].count <= 0)                                        \
    {                                                                         \
      if(timer[timer_number].irq == TIMER_TRIGGER_IRQ)                        \
        irq_raised |= IRQ_TIMER##timer_number;                                \
                                                                              \
      if((timer_number != 3) &&                                               \
       (timer[timer_number + 1].status == TIMER_CASCADE))                     \
      {                                                                       \
        timer[timer_number + 1].count--;                                      \
        io_registers[REG_TM0D + (timer_number + 1) * 2] =                     \
         -(timer[timer_number + 1].count);                                    \
      }                                                                       \
                                                                              \
      if(timer_number < 2)                                                    \
      {                                                                       \
        if(timer[timer_number].direct_sound_channels & 0x01)                  \
          sound_timer(timer[timer_number].frequency_step, 0);                 \
                                                                              \
        if(timer[timer_number].direct_sound_channels & 0x02)                  \
          sound_timer(timer[timer_number].frequency_step, 1);                 \
      }                                                                       \
                                                                              \
      timer[timer_number].count +=                                            \
       (timer[timer_number].reload << timer[timer_number].prescale);          \
    }                                                                         \
  }                                                                           \

void reset_machine() {
	reset_gba();
	reg[CHANGED_PC_STATUS] = 1;
}

void init_main()
{
  u32 i;

  skip_next_frame = 0;

  for(i = 0; i < 4; i++)
  {
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

  flush_translation_cache_rom();
  flush_translation_cache_ram();
  flush_translation_cache_bios();
}

u32 init_machine()
{
  qDebugC("execute arm translate",0,0);
  execute_arm_translate(execute_cycles);
  return execute_arm(execute_cycles);
}

u32 no_alpha = 0;

u32 update_gba()
{
  irq_type irq_raised = IRQ_NONE;

  do
  {
	cpu_ticks += execute_cycles;

	reg[CHANGED_PC_STATUS] = 0;

	if(gbc_sound_update)
	{
	  gbc_update_count++;
	  update_gbc_sound(cpu_ticks);
	  gbc_sound_update = 0;
	}

	update_timer(0);
	update_timer(1);
	update_timer(2);
	update_timer(3);

	video_count -= execute_cycles;

	if(video_count <= 0)
	{
	  u32 vcount = io_registers[REG_VCOUNT];
	  u32 dispstat = io_registers[REG_DISPSTAT];

	  if((dispstat & 0x02) == 0)
	  {
		// Transition from hrefresh to hblank
		video_count += (272);
		dispstat |= 0x02;

		if((dispstat & 0x01) == 0)
		{
		  u32 i;
		  if(oam_update)
			oam_update_count++;

		  if(no_alpha)
			io_registers[REG_BLDCNT] = 0;
		  update_scanline();

		  // If in visible area also fire HDMA
		  for(i = 0; i < 4; i++)
		  {
			if(dma[i].start_type == DMA_START_HBLANK)
			  dma_transfer(dma + i);
		  }
		}

		if(dispstat & 0x10)
		  irq_raised |= IRQ_HBLANK;
	  }
	  else
	  {
		// Transition from hblank to next line
		video_count += 960;
		dispstat &= ~0x02;

		vcount++;

		if(vcount == 160)
		{
		  // Transition from vrefresh to vblank
		  u32 i;

		  dispstat |= 0x01;
		  if(dispstat & 0x8)
		  {
			irq_raised |= IRQ_VBLANK;
		  }

		  affine_reference_x[0] =
		   (s32)(address32(io_registers, 0x28) << 4) >> 4;
		  affine_reference_y[0] =
		   (s32)(address32(io_registers, 0x2C) << 4) >> 4;
		  affine_reference_x[1] =
		   (s32)(address32(io_registers, 0x38) << 4) >> 4;
		  affine_reference_y[1] =
		   (s32)(address32(io_registers, 0x3C) << 4) >> 4;

		  for(i = 0; i < 4; i++)
		  {
			if(dma[i].start_type == DMA_START_VBLANK)
			  dma_transfer(dma + i);
		  }
		}
		else

		if(vcount == 228)
		{
		  // Transition from vblank to next screen
		  dispstat &= ~0x01;

		  synchronize_machine();

		  update_gbc_sound(cpu_ticks);

		  process_cheats();
		  vcount = 0;
		}

		if(vcount == (dispstat >> 8))
		{
		  // vcount trigger
		  dispstat |= 0x04;
		  if(dispstat & 0x20)
		  {
			irq_raised |= IRQ_VCOUNT;
		  }
		}
		else
		{
		  dispstat &= ~0x04;
		}

		io_registers[REG_VCOUNT] = vcount;
	  }
	  io_registers[REG_DISPSTAT] = dispstat;
	}

	if(irq_raised)
	  raise_interrupt(irq_raised);

	execute_cycles = video_count;

	check_timer(0);
	check_timer(1);
	check_timer(2);
	check_timer(3);
  } while(reg[CPU_HALT_STATE] != CPU_ACTIVE);

  return execute_cycles;
}

void quit()
{
  // TODO remove quit
  exit(-1);
}

void reset_gba()
{
  init_main();
  init_memory();
  init_cpu();
  reset_sound();
}

#define main_savestate_builder(type)                                          \
void main_##type##_savestate(file_tag_type savestate_file)                    \
{                                                                             \
  file_##type##_variable(savestate_file, cpu_ticks);                          \
  file_##type##_variable(savestate_file, execute_cycles);                     \
  file_##type##_variable(savestate_file, video_count);                        \
  file_##type##_array(savestate_file, timer);                                 \
}                                                                             \

main_savestate_builder(read);
main_savestate_builder(write_mem);
