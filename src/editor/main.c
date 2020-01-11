#include "main.h"
#include "main.gui"

void cursor_update() {
  limit_value(playbar.value, 0, get_song_length());
  if (playbar.value != cur) {
    set_cursor_time(playbar.value);
  }
  cur = get_cursor_time();
  playbar.value = cur;
}

void stop_action() {
  stop_song();
  Buttons[5].text = "#131#";
  state &= ~STATE_PLAY;
  cur = 0;

  set_cursor_time(0);
}

void playbutton_action() {
  if (IsKeyDown(KEY_A) || Buttons[4].value == 1) {
    playbar.value -= jumpDistance;
  }
  if (IsKeyDown(KEY_D) || Buttons[6].value == 1) {
    playbar.value += jumpDistance;
  }
  // To treat the situation that the cursor meet the end of the song and jump back to zero
  if (cur == 0) {
    stop_action();
    /***
    state &= ~STATE_PLAY;
    Buttons[5].text = "#131#";
    ***/
  }
  if (state & STATE_PLAY) {
    if (IsKeyReleased(KEY_SPACE) || Buttons[5].value == 1) {
      pause_song();
      Buttons[5].text = "#131#";
      state ^= STATE_PLAY;
    }
  } else {
    if (IsKeyReleased(KEY_SPACE) || Buttons[5].value == 1) {
      play_song();
      set_cursor_time(cur);
      Buttons[5].text = "#132#";
      state |= STATE_PLAY;
    }
  }
  
  if (Buttons[7].value == 1) {
    stop_action();
    playbar.value = 0;
  }
  
  // openfile button
  if (Buttons[3].value == 1) {
    stop_action();
    playbar.value = 0;
    sound_unload();
    clear_score_list();
    strcpy(DragFileWindow.text, "Drag your audio file here.");
    playbar.max = 0;
    state ^= STATE_FILELOADED;
    state ^= STATE_FILEOPENED;
  }
}

void draw_play_control_pannel() {
  unsigned char i;
  limit_value(playbar.value, playbar.min, playbar.max);
  playbar.value = GuiSliderBar(playbar.bounds,
			       (state & STATE_PLAY)?"Playing":((cur>0)?"Pausing":"Stopped"),
			       TextFormat("%02d:%02d/%02d:%02d", (int)(playbar.value / 60000),(int)(playbar.value / 1000) % 60, (int)(playbar.max / 60000), (int)(playbar.max / 1000) % 60),
			       playbar.value, playbar.min, playbar.max);
  for (i = 3; i < 8; i++) Buttons[i].value = GuiButton(Buttons[i].bounds, Buttons[i].text);
  GuiLabel(namelabel.bounds, DragFileWindow.text);
}

void draw_window_control() {
  unsigned char i;
  for (i = 0; i < 3; i++) GuiGroupBox(GroupBox[i].bounds, GroupBox[i].text);
    
  GuiStatusBar(StatusBar.bounds, StatusBar.text);
}

void draw_open_file_window() {
      /*** open file window ***/
  GuiEnable();
  if (!(state & STATE_FILEOPENED)) {
    DragFileWindow.ret = GuiMessageBox(DragFileWindow.bounds, DragFileWindow.wintitle, DragFileWindow.text, DragFileWindow.buttons);
    GuiDisable();
  }
    /*** open file window end ***/
}

void draw_mode_panel() {
  /*** Mode choose ***/
  if (state & STATE_PLAY) {
    GuiLock();
  }
  unsigned char i;
  ModeToggle.active = GuiToggleGroup(ModeToggle.bounds, ModeToggle.text, ModeToggle.active);
    /*** Mode choose end ***/
    /*** note edit ***/
  for (i = 0; i < 2; i++) if(TypeCombo[i].able) TypeCombo[i].active = GuiComboBox(TypeCombo[i].bounds, TypeCombo[i].text, TypeCombo[i].active);
  for (i = 0; i < 2; i++) TimeSpinner[i].editMode ^= GuiSpinner(TimeSpinner[i].bounds, TimeSpinner[i].text, &TimeSpinner[i].value, 0, TimeSpinner[i].sequencelength-1, TimeSpinner[i].editMode);
  for (i = 0; i < 3; i++) Buttons[i].value = GuiButton(Buttons[i].bounds, Buttons[i].text);
    /*** note edit end ***/
  GuiUnlock();
}

void draw_score() {
  unsigned int i;
  /*** Score ***/
  // RMS wave 
  DrawRectangleRec(scrollscore.bounds, DARKGREEN);
  int rms_start = cur / 20 - scrollscore.bounds.height/2*scrollscore.resolution;
  int rms_end = rms_start + scrollscore.bounds.height*scrollscore.resolution;
  int rms_draw_cursor;
  float rms_value;
  for(i = 0; i < scrollscore.bounds.height; i++){
    rms_draw_cursor = rms_start + i * scrollscore.resolution;
    if (rms_draw_cursor >= 0 && rms_draw_cursor < get_rms_length()) {
      rms_value = get_rms(rms_draw_cursor) * scrollscore.bounds.width;
      DrawRectangle(scrollscore.bounds.x+(scrollscore.bounds.width - rms_value)/2, scrollscore.bounds.y+scrollscore.bounds.height-i-1, rms_value, 1, LIME);
    }
  }

  // [TODO] notes
    
  // GridLine 
  DrawLine(scrollscore.bounds.x, scrollscore.bounds.y+scrollscore.bounds.height/2, scrollscore.bounds.x+scrollscore.bounds.width, scrollscore.bounds.y+scrollscore.bounds.height/2, YELLOW);
  for (i = 1; i < 6; i++) {
    DrawLine(scrollscore.bounds.x + scrollscore.bounds.width*i/6, scrollscore.bounds.y, scrollscore.bounds.x + scrollscore.bounds.width*i/6, scrollscore.bounds.y+scrollscore.bounds.height, LIGHTGRAY);
  }
  // [TODO] Score information 
  DrawText(FormatText("%d, %05.1f", cur/1000, rms_value), 25, 25, 12, LIGHTGRAY);
  /*** score end ***/
  
}

void game_init() {
  InitWindow(screenWidth, screenHeight, title);
  InitAudioDevice();
  SetTargetFPS(60);
}

void game_update() {
  mousePosition = GetMousePosition();
  if (state & STATE_FILEOPENED) {
    cursor_update();
    playbutton_action();

    if (!(state & STATE_PLAY)) {
      if 

      
    }
  } else {
    if (state & STATE_FILELOADED) {
      if (DragFileWindow.ret == 2) {
	sound_unload();
	strcpy(DragFileWindow.text, "Drag your audio file here.");
	playbar.max = 0;
	state ^= STATE_FILELOADED;

      } else if (DragFileWindow.ret == 1) {
	state |= STATE_FILEOPENED;
      } 
    } else {
      if (CheckCollisionPointRec(mousePosition, DragFileWindow.bounds)) {
	if (IsFileDropped()) {
	  int count;
	  char ** droppedFilenames = GetDroppedFiles(&count);
	  for (int i = 0; i < count; i++) {
	    if (is_music_file(droppedFilenames[i])) {
	      sound_load_song(droppedFilenames[i]);
	      strcpy(DragFileWindow.text, TextFormat("%.20s", GetFileNameWithoutExt(droppedFilenames[i])));
	      read_score_file(TextFormat("%s.wind", DragFileWindow.text));
	      state |= STATE_FILELOADED;
	      TimeSpinner[0].sequencelength = get_song_length()/20;
	      playbar.max = get_song_length();
	      break;
	    }
	  }
	}
      }
    }
  }
  ClearDroppedFiles();
}



void game_draw() {
  BeginDrawing(); {
    ClearBackground(RAYWHITE);
    draw_score();

    draw_mode_panel();
    draw_play_control_pannel();
    draw_window_control();
    draw_open_file_window();
  } EndDrawing();
  
}

void game_exit() {
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
