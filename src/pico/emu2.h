// (c) Copyright 2010 javicq, All rights reserved.
// Free for non-commercial use.

// For commercial use, separate licencing terms must be obtained.

#define _P_ENABLE_KEY 0x01
#define _P_ENABLE_TS 0x02
#define _P_ENABLE_ACC 0x04

extern unsigned char *rom_data;
extern int state_slot;
extern int config_slot, config_slot_current;
extern unsigned char *movie_data;


int   emu_ReloadRom(void);
int   emu_SaveLoadGame(int load, int sram);
int   emu_ReadConfig(int game, int no_defaults);
int   emu_WriteConfig(int game);
char *emu_GetSaveFName(int load, int is_sram, int slot);
int   emu_checkSaveFile(int slot);
void  emu_setSaveStateCbs(int gz);
int   emu_cdCheck(int *pregion);
int   emu_findBios(int region, char **bios_file);
