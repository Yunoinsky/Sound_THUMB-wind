#include "main.h"
#include "main.gui"

#define STATE_FILELOADED        (1 << 0)
#define STATE_PLAY              (1 << 1)

unsigned char state;

inline void game_init() {
  InitWindow(screenWidth, screenHeight, title);
  const char* sound_url = "music_1.mp3";
  audio_init();
  sound_load_song(sound_url);
  SetTargetFPS(60);
}

inline void game_update() {
  mousePosition = GetMousePosition();

}

inline void game_draw() {
  BeginDrawing(); {
    ClearBackground(RAYWHITE);
    type_togglegroup.active = GuiToggleGroup(type_togglegroup.bounds, type_togglegroup.text, type_togglegroup.active);
    for (unsigned int i = 0; i < 3; i ++) GuiGroupBox(GroupBox[i].bounds, GroupBox[i].text);
    GuiStatusBar(StatusBar.bounds, StatusBar.text);
  } EndDrawing();
}

inline void game_exit() {
  CloseAudioDevice();
  CloseWindow();
}

int main (int argc, char ** argv) {
  game_init();
  bool close;
  while (!WindowShouldClose()) {
    game_update();
    game_draw();
  }
  game_exit();
  return 0;
}
