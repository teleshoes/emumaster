// (c) Copyright 2006-2007 notaz, All rights reserved.
// Free for non-commercial use.

// For commercial use, separate licencing terms must be obtained.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // tolower

#include "emu2.h"
#include "pico.h"
#include "cheat.h"
#include <zlib.h>

/* checks if romFileName points to valid MegaCD image
 * if so, checks for suitable BIOS */
int emu_cdCheck(int *pregion)
{
	return 0;

/* TODO unsigned char buf[32];
	pm_file *cd_f;
	int type = 0, region = 4; // 1: Japan, 4: US, 8: Europe

	cd_f = pm_open(romFileName);
	if (!cd_f) return 0; // let the upper level handle this

	if (pm_read(buf, 32, cd_f) != 32) {
		pm_close(cd_f);
		return 0;
	}

	if (!strncasecmp("SEGADISCSYSTEM", (char *)buf+0x00, 14)) type = 1;       // Sega CD (ISO)
	if (!strncasecmp("SEGADISCSYSTEM", (char *)buf+0x10, 14)) type = 2;       // Sega CD (BIN)
	if (type == 0) {
		pm_close(cd_f);
		return 0;
	}

	// it seems we have a CD image here. Try to detect region now..
	pm_seek(cd_f, (type == 1) ? 0x100+0x10B : 0x110+0x10B, SEEK_SET);
	pm_read(buf, 1, cd_f);
	pm_close(cd_f);

	if (buf[0] == 0x64) region = 8; // EU
	if (buf[0] == 0xa1) region = 1; // JAP

	lprintf("detected %s Sega/Mega CD image with %s region\n",
		type == 2 ? "BIN" : "ISO", region != 4 ? (region == 8 ? "EU" : "JAP") : "USA");

	if (pregion != NULL) *pregion = region;

	return type;*/
}

int emu_ReloadRom(void)
{
/* TODO	unsigned int rom_size = 0;
	char *used_rom_name = romFileName;
	pm_file *rom;
	int ret, cd_state, cd_region, cfg_loaded = 0;

	// check for MegaCD image
	cd_state = emu_cdCheck(&cd_region);
	if (cd_state > 0)
	{
		// valid CD image, check for BIOS..

		// we need to have config loaded at this point
		ret = emu_ReadConfig(1, 1);
		if (!ret) emu_ReadConfig(0, 1);
		cfg_loaded = 1;

		if (PicoRegionOverride) {
			cd_region = PicoRegionOverride;
			lprintf("overrided region to %s\n", cd_region != 4 ? (cd_region == 8 ? "EU" : "JAP") : "USA");
		}
		if (!emu_findBios(cd_region, &used_rom_name)) {
			// bios_help() ?
			return 0;
		}

		PicoMCD |= 1;
	}

	rom = pm_open(used_rom_name);
	if( (ret = PicoCartLoad(rom, &rom_data, &rom_size)) ) {
		pm_close(rom);
		return 0;
	}
	pm_close(rom);

	// detect wrong files (Pico crashes on very small files), also see if ROM EP is good
	if(rom_size <= 0x200 || strncmp((char *)rom_data, "Pico", 4) == 0 ||
	  ((*(unsigned char *)(rom_data+4)<<16)|(*(unsigned short *)(rom_data+6))) >= (int)rom_size) {
		if (rom_data) free(rom_data);
		rom_data = 0;
		return 0;
	}

	// load config for this ROM (do this before insert to get correct region)
	if (!cfg_loaded) {
		ret = emu_ReadConfig(1, 1);
		if (!ret) emu_ReadConfig(0, 1);
	}

	lprintf("PicoCartInsert(%p, %d);\n", rom_data, rom_size);
	if(PicoCartInsert(rom_data, rom_size)) {
		return 0;
	}

	// insert CD if it was detected
	if (cd_state > 0) {
		ret = Insert_CD(romFileName, cd_state == 2);
	}

		PicoOpt &= ~0x10000;

	return 1;*/
}
