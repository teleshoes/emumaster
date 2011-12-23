#include "machine.h"
#include "pico.h"
#include "ym2612.h"
#include "sn76496.h"

static void cycloneSl(const QString &name, Cyclone *cyclone) {
	u8 data[0x60];
	memset(&data, 0, sizeof(data));

	if (emsl.save)
		CyclonePack(cyclone, data);

	emsl.array(name, data, sizeof(data));

	if (!emsl.save)
		CycloneUnpack(cyclone, data);
}
static void z80Sl(const QString &name) {
	u8 data[0x60];
	memset(&data, 0, sizeof(data));

	if (emsl.save)
		z80_pack(data);

	emsl.array(name, data, sizeof(data));

	if (!emsl.save)
		z80_unpack(data);
}

static void picoCdSl() {
	// TODO
}

void PicoMachine::sl() {
	emsl.begin("pico");

	// memory areas
	emsl.array("ram", Pico.ram, sizeof(Pico.ram));
	emsl.array("vram", Pico.vram, sizeof(Pico.vram));
	emsl.array("zram", Pico.zram, sizeof(Pico.zram));
	emsl.array("cram", Pico.cram, sizeof(Pico.cram));
	emsl.array("vsram", Pico.vsram, sizeof(Pico.vsram));

	// cpus
	cycloneSl("cpu.m68k", &PicoCpuCM68k);
	if (PicoMCD & 1)
		cycloneSl("cpu.s68k", &PicoCpuCS68k);
	z80Sl("cpu.z80");

	emsl.array("misc", &Pico.m, sizeof(Pico.m));
	emsl.array("video", &Pico.video, sizeof(Pico.video));

	sn76496.sl("psg");

	void *ym2612_regs = YM2612GetRegs();
	emsl.array("fm", ym2612_regs, 0x200+4);
	if (!emsl.save)
		YM2612PicoStateLoad(); // reload YM2612 state from it's regs

	if (!emsl.save)
		Pico.m.dirtyPal=1;

	emsl.end();

	if (PicoMCD & 1)
		picoCdSl();

	/* TODO save sram
	FILE *sramFile;
	int sram_size;
	unsigned char *sram_data;
	int truncate = 1;
	if (PicoMCD&1) {
		if (PicoOpt&0x8000) { // MCD RAM cart?
			sram_size = 0x12000;
			sram_data = SRam.data;
			if (sram_data)
				memcpy32((int *)sram_data, (int *)Pico_mcd->bram, 0x2000/4);
		} else {
			sram_size = 0x2000;
			sram_data = Pico_mcd->bram;
			truncate  = 0; // the .brm may contain RAM cart data after normal brm
		}
	} else {
		sram_size = SRam.end-SRam.start+1;
		if(Pico.m.sram_reg & 4) sram_size=0x2000;
		sram_data = SRam.data;
	}
	if (!sram_data) return 0; // SRam forcefully disabled for this game

	if (load) {
		sramFile = fopen(saveFname, "rb");
		if(!sramFile) return -1;
		fread(sram_data, 1, sram_size, sramFile);
		fclose(sramFile);
		if ((PicoMCD&1) && (PicoOpt&0x8000))
			memcpy32((int *)Pico_mcd->bram, (int *)sram_data, 0x2000/4);
	} else {
		// sram save needs some special processing
		// see if we have anything to save
		for (; sram_size > 0; sram_size--)
			if (sram_data[sram_size-1]) break;

		if (sram_size) {
			sramFile = fopen(saveFname, truncate ? "wb" : "r+b");
			if (!sramFile) sramFile = fopen(saveFname, "wb"); // retry
			if (!sramFile) return -1;
			ret = fwrite(sram_data, 1, sram_size, sramFile);
			ret = (ret != sram_size) ? -1 : 0;
			fclose(sramFile);
		}
	}*/
}

int PicoCdSaveState()
{
/*	// TODO Pico_mcd->m.audio_offset = mp3_get_offset();
	if (Pico_mcd->s68k_regs[3]&4) // 1M mode?
		wram_1M_to_2M(Pico_mcd->word_ram2M);
	Pico_mcd->m.hint_vector = *(unsigned short *)(Pico_mcd->bios + 0x72);

	CHECKED_WRITE_BUFF(CHUNK_PRG_RAM,  Pico_mcd->prg_ram);
	CHECKED_WRITE_BUFF(CHUNK_WORD_RAM, Pico_mcd->word_ram2M); // in 2M format
	CHECKED_WRITE_BUFF(CHUNK_PCM_RAM,  Pico_mcd->pcm_ram);
	CHECKED_WRITE_BUFF(CHUNK_BRAM,     Pico_mcd->bram);
	CHECKED_WRITE_BUFF(CHUNK_GA_REGS,  Pico_mcd->s68k_regs); // GA regs, not CPU regs
	CHECKED_WRITE_BUFF(CHUNK_PCM,      Pico_mcd->pcm);
	CHECKED_WRITE_BUFF(CHUNK_CDD,      Pico_mcd->cdd);
	CHECKED_WRITE_BUFF(CHUNK_CDC,      Pico_mcd->cdc);
	CHECKED_WRITE_BUFF(CHUNK_SCD,      Pico_mcd->scd);
	CHECKED_WRITE_BUFF(CHUNK_RC,       Pico_mcd->rot_comp);
	CHECKED_WRITE_BUFF(CHUNK_MISC_CD,  Pico_mcd->m);

	if (Pico_mcd->s68k_regs[3]&4) // convert back
		wram_2M_to_1M(Pico_mcd->word_ram2M);

	return 0;*/
}

int PicoCdLoadState()
{
	/*unsigned char buff[0x60];
	while (!areaEof(file))
	{
		switch (buff[0])
		{
		case CHUNK_PRG_RAM:	CHECKED_READ_BUFF(Pico_mcd->prg_ram); break;
		case CHUNK_WORD_RAM:CHECKED_READ_BUFF(Pico_mcd->word_ram2M); break;
		case CHUNK_PCM_RAM:	CHECKED_READ_BUFF(Pico_mcd->pcm_ram); break;
		case CHUNK_BRAM:	CHECKED_READ_BUFF(Pico_mcd->bram); break;
		case CHUNK_GA_REGS:	CHECKED_READ_BUFF(Pico_mcd->s68k_regs); break;
		case CHUNK_PCM:		CHECKED_READ_BUFF(Pico_mcd->pcm); break;
		case CHUNK_CDD:		CHECKED_READ_BUFF(Pico_mcd->cdd); break;
		case CHUNK_CDC:		CHECKED_READ_BUFF(Pico_mcd->cdc); break;
		case CHUNK_SCD:		CHECKED_READ_BUFF(Pico_mcd->scd); break;
		case CHUNK_RC:		CHECKED_READ_BUFF(Pico_mcd->rot_comp); break;
		case CHUNK_MISC_CD:	CHECKED_READ_BUFF(Pico_mcd->m); break;
		}
	}

	if (Pico_mcd->s68k_regs[3]&4) // 1M mode?
		wram_2M_to_1M(Pico_mcd->word_ram2M);
	PicoMemResetCD(Pico_mcd->s68k_regs[3]);
#ifdef _ASM_CD_MEMORY_C
	if (Pico_mcd->s68k_regs[3]&4)
		PicoMemResetCDdecode(Pico_mcd->s68k_regs[3]);
#endif
	// TODO if (Pico_mcd->m.audio_track > 0 && Pico_mcd->m.audio_track < Pico_mcd->TOC.Last_Track)
	//	mp3_start_play((FILE*)Pico_mcd->TOC.Tracks[Pico_mcd->m.audio_track].F, Pico_mcd->m.audio_offset);
	// restore hint vector
	*(unsigned short *)(Pico_mcd->bios + 0x72) = Pico_mcd->m.hint_vector;

	return 0;*/
}

