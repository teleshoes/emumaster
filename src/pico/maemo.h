#ifndef __MAEMO_H__
#define __MAEMO_H__

void maemo_init(void);
void maemo_deinit(void);
void maemo_quit();

/* sound */
void maemo_start_sound(int rate, int bits, int stereo);
void maemo_sound_write(void *buff, int len);
void maemo_sound_volume(int l, int r);

extern void *md_screen;

#endif
