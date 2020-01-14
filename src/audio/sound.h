#ifndef __SOUND_H__
#define __SOUND_H__

#include "../others/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Sound sound;
float * rms_data;

bool soundloaded;
int rms_length;

#define CHUNK_SIZE   5

void calc_rms(Wave wave);
float get_rms(int chunk);
int get_rms_length();
void sound_load_song(const char* sound_url);
void sound_unload();
int get_song_length();
int get_cursor_time();
void set_cursor_time(int tar_time);
void resume_song();
void play_song();
void pause_song();
void stop_song();

bool is_music_file(const char* filename);

#endif
