#include "main.h"
#include "main.gui"


inline void game_init() {
  InitWindow(screenWidth, screenHeight, title);
  InitAudioDevice();
  
  SetTargetFPS(60);
}

inline void game_update() {
  mousePosition = GetMousePosition();
}
int m = 0;
int f = 0;
bool x;
char str[40];
inline void game_draw() {
  BeginDrawing(); {
    ClearBackground(RAYWHITE);
    /***
    type_togglegroup.active = GuiToggleGroup(type_togglegroup.bounds, type_togglegroup.text, type_togglegroup.active);
    for (unsigned int i = 0; i < 3; i ++) {
      GuiGroupBox(GroupBox[i].bounds, GroupBox[i].text);
    }
    GuiStatusBar(StatusBar.bounds, "OK");
    ***/
    GuiGrid((Rectangle){ 80, 80, 300, 300 }, 50, 2);
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
