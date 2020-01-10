#ifndef __SOUND_H__
#define __SOUND_H__

#include "../others/raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE    32
#define CHANNELS        2

AudioStream stream;
Sound sound;

void audio_init();
void sound_load_song(const char* sound_url);
bool is_music_file(const char* filename);

#endif
