#include "sound.h"

void calc_rms(float * rawdata) {
  // 20 ms / rmschunk
  int chunksize = sound.stream.sampleRate / 50 * sound.stream.channels;
  float total = 0;
  int len = sound.sampleCount / chunksize + 1;
  rms_length = len;
  int i = 0;
  int j;
  rms_data = (float *)malloc(sizeof(float)*len);
  while (i < len - 1) {
    for (j = 0; j < chunksize; j++) {
      total += (rawdata[i * chunksize + j] * rawdata[i * chunksize + j]);
    }
    rms_data[i] = sqrtf(total / chunksize);
    i++;
    total = 0;
  }
  int restdatanum = sound.sampleCount - (i - 1) * chunksize;
  
  for (j = 0; j < restdatanum; j ++) {
    total += (rawdata[i * chunksize + j] * rawdata[i * chunksize + j]);
  }
  rms_data[i] = sqrtf(total / restdatanum);
}

float get_rms(int chunk) {
  if (soundloaded) return rms_data[chunk];
  else return 0.0f;
}

int get_rms_length() {
  if (soundloaded) return rms_length;
  else return 0;
}

void sound_load_song(const char* sound_url) {
  Wave wave = LoadWave(sound_url);
  sound = LoadSoundFromWave(wave);
  calc_rms(GetWaveData(wave));
  UnloadWave(wave);
  soundloaded = true;
}

void sound_unload() {
  soundloaded = false;  
  free(rms_data);
  UnloadSound(sound);
}

int get_song_length() {
  if (soundloaded) return (int)((long long)sound.sampleCount * 1000 / sound.stream.sampleRate / 2);
  else return 0;
}

int get_cursor_time() {
  if (soundloaded) return (int)((long long)sound.stream.buffer->frameCursorPos * 1000 / sound.stream.sampleRate / 2 * 2);
  else return 0;
}

void set_cursor_time(int tar_time) {
  sound.stream.buffer->frameCursorPos = (int)((long long)tar_time * sound.stream.sampleRate / 1000);
}

void resume_song() {
  ResumeSound(sound);
}

void play_song() {
  PlaySound(sound);
}

void pause_song() {
  PauseSound(sound);
  
}

void stop_song() {
  StopSound(sound);
}

bool is_music_file(const char* filename) {
  return (IsFileExtension(filename, ".mp3") ||
	  IsFileExtension(filename, ".wav") ||
	  IsFileExtension(filename, ".ogg") ||
	  IsFileExtension(filename, ".flac"));
}

