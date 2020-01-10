#include "sound.h"
bool is_music_file(const char* filename) {
  return (IsFileExtension(filename, ".mp3") ||
	  IsFileExtension(filename, ".wav") ||
	  IsFileExtension(filename, ".ogg"));
}

void audio_init() {
  InitAudioDevice();
  stream = InitAudioStream(SAMPLE_RATE, SAMPLE_SIZE, CHANNELS);
}

void sound_load_song(const char* sound_url) {
  Wave wave = LoadWave(sound_url);
  WaveFormat(&wave, SAMPLE_RATE, SAMPLE_SIZE, CHANNELS);
  sound = LoadSoundFromWave(wave);
  PlaySoundMulti(sound);
  UnloadWave(wave);
}




