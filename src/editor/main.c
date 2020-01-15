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
  if (Buttons[4].value == 1) {
    playbar.value -= 50 * jumpDistance;
  }
  if (Buttons[6].value == 1) {
    playbar.value += 50 * jumpDistance;
  }  
  if (IsKeyDown(KEY_A)) {
    playbar.value -= jumpDistance;
  }
  if (IsKeyDown(KEY_Q)) {
    playbar.value -= jumpDistance/20;
  }
  if (IsKeyDown(KEY_D)) {
    playbar.value += jumpDistance;
  }
  if (IsKeyDown(KEY_E)) {
    playbar.value += jumpDistance/20;
  }
  if (CheckCollisionPointRec(mousePosition, scrollscore.bounds) || CheckCollisionPointRec(mousePosition, playbar.bounds)) {
    playbar.value += GetMouseWheelMove() * jumpDistance;
  }
  
  // To treat the situation that the cursor meet the end of the song and jump back to zero
  if (cur == 0) {
    stop_action();
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

int modetoggle_to_notetype(int modetoggle) {
  switch (modetoggle) {
  case 2: return CLK_NOTE;
  case 3: return SLP_NOTE;
  case 4: return SHD_NOTE;
  case 5: return SDG_NOTE;
  case 6: return PDG_NOTE;
  case 7: return END_NOTE;
  case 8: return FUN_NODE;
  default: return -1;
  }
}

int notetype_to_modetoggle(int type) {
  switch (type) {
  case CLK_NOTE: return 2;
  case SLP_NOTE: return 3;
  case SHD_NOTE: return 4;
  case SDG_NOTE: return 5;
  case PDG_NOTE: return 6;
  case END_NOTE: return 7;
  case FUN_NODE: return 8;
  default: return -1;
  }
}
  
Rectangle get_note_rect(Note note, unsigned char track) {
  // the height of note is 5 pixels
  int coor = ((int)note.time - cur) / CHUNK_SIZE / scrollscore.resolution;
  coor = scrollscore.bounds.y + scrollscore.bounds.height/2 - coor;
  return (Rectangle){  scrollscore.bounds.x + track * scrollscore.bounds.width / 6, coor - 10, scrollscore.bounds.width/6, 10 };
}

void DrawSingleNoteEx(Note note, unsigned char track, bool hl) {
  float alpha = hl? 0.8f : 0.6f;
  Color color;
  switch(note.type) {
  case CLK_NOTE: color = SKYBLUE; break;
  case SLP_NOTE: color = PINK; break;
  case SHD_NOTE: color = BROWN; break;
  case SDG_NOTE: color = PURPLE; break;
  case PDG_NOTE: color = PURPLE; break;
  case END_NOTE: color = DARKGRAY; break;
  case FUN_NODE: color = YELLOW; break;
  default: break;
  }
  DrawRectangleRounded(get_note_rect(note, track), 1.0, 8, Fade(color, alpha));
  if (hl) {
    DrawRectangleRoundedLines(get_note_rect(note, track), 1.0, 8, 2, Fade(RAYWHITE, 0.9f));
  } else {
    DrawRectangleRoundedLines(get_note_rect(note, track), 1.0, 8, 2, Fade(LIGHTGRAY, 0.5f));
  }
}

void UpdateandDrawSingleNote(NoteNode* node, unsigned char track) {
  Rectangle r = get_note_rect(node->note, track);
  if (((ModeToggle.active == 0) ||
       (ModeToggle.active == 1)) &&
      (node == selected_notenode ||
       CheckCollisionPointRec(mousePosition, r))) {
    DrawSingleNoteEx(node->note, track, true);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      selected_notenode = node;
      selected_notetrack = track;
      if (CheckCollisionPointRec(mousePosition, r)){
	new_node_selected = true;
      }
    }
  } else {
    DrawSingleNoteEx(node->note, track, false);
  }
}

void DrawDragLinkRoad(NoteNode* prev, NoteNode* next, unsigned char trackprev, unsigned char tracknext) {
  Rectangle rp = get_note_rect(prev->note, trackprev);
  Rectangle rn = get_note_rect(next->note, tracknext);
  DrawTriangle((Vector2){ rp.x, rp.y}, (Vector2){ rp.x+rp.width, rp.y }, (Vector2){ rn.x, rn.y }, Fade(PINK, 0.3f));
  DrawTriangle((Vector2){ rn.x, rn.y}, (Vector2){ rp.x+rp.width, rp.y }, (Vector2){ rn.x+rn.width, rn.y }, Fade(PINK, 0.3f));
}

void DrawSlipRoad(NoteNode* node, unsigned char track) {
  Rectangle r = get_note_rect(node->note, track);
  switch (node->note.direction) {
  case NOTE_LEFT: DrawTriangle((Vector2){ r.x - r.width / 2, r.y + r.height / 2 }, (Vector2){ r.x, r.y + r.height}, (Vector2){ r.x, r.y }, Fade(PINK, 0.5f)); break;
  case NOTE_MEDIUM: DrawTriangle((Vector2){ r.x, r.y }, (Vector2){ r.x + r.width, r.y }, (Vector2){ r.x + r.width/2, r.y - r.height}, Fade(PINK, 0.5f)); break;
  case NOTE_RIGHT: DrawTriangle((Vector2){ r.x + r.width, r.y }, (Vector2){ r.x + r.width, r.y + r.height}, (Vector2){ r.x + r.width/2*3, r.y + r.height/2 }, Fade(PINK, 0.5f)); break;
  default: break;
  }
}

void DrawHoldRoad(NoteNode * prev, NoteNode * next, unsigned char track) {
  Rectangle rp = get_note_rect(prev->note, track);
  Rectangle rn = get_note_rect(next->note, track);
  DrawRectangle(rn.x, rn.y, rn.width, rp.y-rn.y, Fade(BROWN, 0.8f));
}

void DrawFunctionRoad(NoteNode * prev, NoteNode * next, unsigned char track) {
  Rectangle rp = get_note_rect(prev->note, track);
  Rectangle rn = get_note_rect(next->note, track);
  switch(prev->note.direction) {
  case NOTE_DISABLE:  DrawRectangle(rn.x, rn.y, rn.width, rp.y-rn.y, Fade(GRAY, 0.8f)); break;
  case NOTE_CADENZA:  DrawRectangle(rn.x, rn.y, rn.width, rp.y-rn.y, Fade(GOLD, 0.2f)); break;
  default: break;
  }
}

void notes_draw() {
  unsigned int i;
  // find out notes
  Rectangle note_to_draw;
  int start_time = cur - scrollscore.bounds.height * scrollscore.resolution * CHUNK_SIZE / 2;
  int end_time = cur + scrollscore.bounds.height * scrollscore.resolution * CHUNK_SIZE / 2;
  if (Notelists[0] != NULL) {
    NoteNode * notecur;
    NoteNode * tempcur;

    for (i = 0; i < TRACK_NUM; i++) {
      notecur = Notelists[i]->next;
      tempcur = notecur;
      while (notecur != NULL && ((int)notecur->note.time <= start_time)) {
	tempcur = notecur;
	notecur = notecur->next;
      }
      draw_prevstartnote[i] = tempcur;


      while (notecur != NULL && ((int)notecur->note.time <= end_time)) {
	notecur = notecur->next;
      }
      draw_endnote[i] = notecur;
    }

    unsigned char trackcur;
    NoteNode * dragcur;
    for (i = 0; i < TRACK_NUM; i++) {
      notecur = Notelists[i];
      while (notecur != NULL && notecur != draw_endnote[i]) {
	if (notecur->note.type == FUN_NODE &&
	    (notecur->note.direction == NOTE_DISABLE ||
	     notecur->note.direction == NOTE_CADENZA)) {
	  dragcur = notecur->next;
	  while (dragcur != NULL) {
	    if (dragcur->note.type == FUN_NODE && (is_end_of_fun(notecur->note.direction, dragcur->note.direction))) {
	      DrawFunctionRoad(notecur, dragcur, i);
	      break;
	    }
	    dragcur = dragcur->next;
	  }
	}
	notecur = notecur->next;
      }
    }
    
    for (i = 0; i < TRACK_NUM; i++) {
      notecur = draw_prevstartnote[i];
      while (notecur != NULL && notecur != draw_endnote[i]) {
	// draw background for drag note
	switch (notecur->note.type) {
	case SLP_NOTE: {
	  DrawSlipRoad(notecur, i);
	  break;
	}
	case SHD_NOTE: {
	  if (notecur->next != NULL && notecur->next->note.type == END_NOTE) {
	    DrawHoldRoad(notecur, notecur->next, i);
	  }
	  break;
	}
	case SDG_NOTE:
	case PDG_NOTE: {
	  switch (notecur->note.direction) {
	  case NOTE_LEFT: trackcur = yumax(0, i-1); break;
	  case NOTE_MEDIUM: trackcur = i; break;
	  case NOTE_RIGHT: trackcur = yumin(TRACK_NUM, i+1); break;
	  default: break;
	  }
	  dragcur = draw_prevstartnote[trackcur];
	  while (dragcur != NULL && (int)dragcur->note.time <= (int)notecur->note.time) {
	    dragcur = dragcur->next;
	  }
	  if (dragcur != NULL && (dragcur->note.type == PDG_NOTE || dragcur->note.type == END_NOTE) && ((int)dragcur->note.time > start_time)) {
	    DrawDragLinkRoad(notecur, dragcur, i, trackcur);
	  }
	  break;
	}
	default: break;
	}
	notecur = notecur->next;
      }
    }
    for (i = 0; i < TRACK_NUM; i++) {
      notecur = draw_prevstartnote[i];
      while (notecur != NULL && notecur != draw_endnote[i]) {
	if (notecur->note.time != 0) UpdateandDrawSingleNote(notecur, i);
	notecur = notecur->next;
      }
    }
  }
}

void draw_play_control_pannel() {
  unsigned char i;
  limit_value(playbar.value, playbar.min, playbar.max);
  playbar.value = GuiSliderBar(playbar.bounds,
			       (state & STATE_PLAY)?"Playing":((cur>0)?"Pausing":"Stopped"),
			       TextFormat("%02d:%02d/%02d:%02d", (int)(playbar.value / 60000), (int)(playbar.value / 1000) % 60, (int)(playbar.max / 60000), (int)(playbar.max / 1000) % 60),
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
    DragFileWindow.ret = GuiMessageBox(DragFileWindow.bounds,
				       DragFileWindow.wintitle,
				       DragFileWindow.text,
				       DragFileWindow.buttons);
    GuiDisable();
  }
    /*** open file window end ***/
}

void draw_mode_panel() {
  if (state & STATE_PLAY) {
    GuiDisable();
  }
  unsigned char i;
  /*** Mode choose ***/
  ModeToggle.active = GuiToggleGroup(ModeToggle.bounds, ModeToggle.text, ModeToggle.active);
  /*** Mode choose end ***/

  if (!(ModeToggle.active == 0 && selected_notenode != NULL)) {
    GuiDisable();
  }
  /*** note edit pannel***/
  TypeCombo[0].active = GuiComboBox(TypeCombo[0].bounds, TypeCombo[0].text, TypeCombo[0].active);
  
  if (TypeCombo[0].active == 1 ||
      TypeCombo[0].active == 3 ||
      TypeCombo[0].active == 4)
    TypeCombo[1].active = GuiComboBox(TypeCombo[1].bounds, TypeCombo[1].text, TypeCombo[1].active);
  else if (TypeCombo[0].active == 6)
    TypeCombo[1].active = GuiComboBox(TypeCombo[1].bounds,
				      "disable;enable;cadenza;endcadenza",
				      TypeCombo[1].active);
	   
  for (i = 0; i < 2; i++)
    TimeSpinner[i].editMode ^= GuiSpinner(TimeSpinner[i].bounds,
					  TimeSpinner[i].text,
					  &TimeSpinner[i].value,
					  0,
					  TimeSpinner[i].sequencelength-1,
					  TimeSpinner[i].editMode);
  for (i = 0; i < 3; i++)
    Buttons[i].value = GuiButton(Buttons[i].bounds, Buttons[i].text);
  /*** note edit pannel end ***/

  if (state & STATE_FILEOPENED) {
    GuiEnable();
  }
}

void mousedraw() {
  if (ModeToggle.active > 1 &&
      ModeToggle.active < 9 &&
      CheckCollisionPointRec(mousePosition, scrollscore.bounds)) {
    HideCursor();
    int newtime = limit_num(((scrollscore.bounds.y + scrollscore.bounds.height/2 - mousePosition.y) * scrollscore.resolution * CHUNK_SIZE + cur), 0, playbar.max);
    int newtrack = limit_num(((mousePosition.x - scrollscore.bounds.x) / (scrollscore.bounds.width / 6)), 0, TRACK_NUM);
    DrawSingleNoteEx((Note) {modetoggle_to_notetype(ModeToggle.active), newtime, TypeCombo[1].active }, newtrack, true);
  } else {
    ShowCursor();
  }
}

void draw_score() {
  unsigned int i;
  /*** Score ***/
  // RMS wave

  DrawRectangleRec(scrollscore.bounds, DARKGREEN);
  // 20 ms / rmschunk
  int rms_start = cur / CHUNK_SIZE - scrollscore.bounds.height/2*scrollscore.resolution;
  int rms_end = rms_start + scrollscore.bounds.height*scrollscore.resolution;
  int rms_draw_cursor;
  float rms_value;
  for(i = 0; i < scrollscore.bounds.height; i++){
    rms_draw_cursor = rms_start + i * scrollscore.resolution;
    if (rms_draw_cursor >= 0 && rms_draw_cursor < get_rms_length()) {
      rms_value = get_rms(rms_draw_cursor) * scrollscore.bounds.width;
      DrawRectangle(scrollscore.bounds.x+(scrollscore.bounds.width - rms_value)/2,
		    scrollscore.bounds.y+scrollscore.bounds.height-i-1,
		    rms_value,
		    1,
		    LIME);
    }
  }

  //#ifdef _WIN32
  BeginScissorMode(scrollscore.bounds.x, scrollscore.bounds.y, scrollscore.bounds.width, scrollscore.bounds.height);
  //#elif __APPLE__
  //  BeginScissorMode(scrollscore.bounds.x * 2, scrollscore.bounds.y * 2 -  screenHeight, scrollscore.bounds.width * 2, scrollscore.bounds.height * 2); // do not need this line in application bundle
  //#endif
  
  notes_draw();
  mousedraw();
  EndScissorMode();
  // GridLine 
  DrawLine(scrollscore.bounds.x,
	   scrollscore.bounds.y+scrollscore.bounds.height/2,
	   scrollscore.bounds.x+scrollscore.bounds.width,
	   scrollscore.bounds.y+scrollscore.bounds.height/2,
	   YELLOW);
  for (i = 1; i < 6; i++) {
    DrawLine(scrollscore.bounds.x + scrollscore.bounds.width*i/6,
	     scrollscore.bounds.y, scrollscore.bounds.x + scrollscore.bounds.width*i/6,
	     scrollscore.bounds.y+scrollscore.bounds.height,
	     LIGHTGRAY);
  }
  // [TODO] Score information 
  DrawText(FormatText("play cursor: %.2f s", cur/1000.0f), 25, 25, 12, LIGHTGRAY);
  DrawText(FormatText("RMS: %05.1f", 100 * get_rms(rms_start+scrollscore.bounds.height/2*scrollscore.resolution)), 200, 25, 12, LIGHTGRAY);
  
  /*** score end ***/
  
}

void editupdate() {
  if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
    if (IsKeyReleased(KEY_ONE)) ModeToggle.active = 0;
    else if (IsKeyReleased(KEY_TWO)) ModeToggle.active = 1;
    else if (IsKeyReleased(KEY_THREE)) ModeToggle.active = 2;
    else if (IsKeyReleased(KEY_FOUR)) ModeToggle.active = 3;
    else if (IsKeyReleased(KEY_FIVE)) ModeToggle.active = 4;
    else if (IsKeyReleased(KEY_SIX)) ModeToggle.active = 5;
    else if (IsKeyReleased(KEY_SEVEN)) ModeToggle.active = 6;
    else if (IsKeyReleased(KEY_EIGHT)) ModeToggle.active = 7;
    else if (IsKeyReleased(KEY_NINE)) ModeToggle.active = 8;
    else if (IsKeyReleased(KEY_ZERO)) ModeToggle.active = 9;
  }
  if (selected_notenode != NULL && ModeToggle.active <= 1 && new_node_selected) {
    new_node_selected = false;
    TypeCombo[0].active = notetype_to_modetoggle(selected_notenode->note.type)-2;
    TypeCombo[1].active = selected_notenode->note.direction;
    TimeSpinner[0].value = selected_notenode->note.time;
    TimeSpinner[1].value = selected_notetrack;
  }
      // in select mode
  if (ModeToggle.active == 0 && selected_notenode != NULL) {
    if (Buttons[0].value == true || IsKeyReleased(KEY_ENTER)) {   // Confirm
      remove_note(selected_notetrack, selected_notenode->note.time, false);
      switch (TypeCombo[0].active) {
      case 0: sflag = insert_note(TimeSpinner[1].value, create_click_note(TimeSpinner[0].value), false); break;
      case 1: sflag = insert_note(TimeSpinner[1].value, create_slip_node(TimeSpinner[0].value, TypeCombo[1].active), false); break;
      case 2: sflag = insert_note(TimeSpinner[1].value, create_start_hold_node(TimeSpinner[0].value), false); break;
      case 3: sflag = insert_note(TimeSpinner[1].value, create_start_drag_node(TimeSpinner[0].value, TypeCombo[1].active), false); break;
      case 4: sflag = insert_note(TimeSpinner[1].value, create_passing_drag_node(TimeSpinner[0].value, TypeCombo[1].active), false); break;
      case 5: sflag = insert_note(TimeSpinner[1].value, create_end_node(TimeSpinner[0].value), false); break;
      case 6: sflag = insert_note(TimeSpinner[1].value, create_fun_node(TimeSpinner[0].value, TypeCombo[1].active), false); break;
      default: sflag = false; break;
      }
      if (!sflag) UndoCommand();
      selected_notenode = NULL;
      sflag = true;
    } else if (Buttons[1].value == true) { // Cancel
	selected_notenode = NULL;
	new_node_selected = false;
    } else if (Buttons[2].value == true || IsKeyReleased(KEY_BACKSPACE)) { // delete
      remove_note(selected_notetrack, selected_notenode->note.time, false);
      selected_notetrack = -1;
      selected_notenode = NULL;
      new_node_selected = false;
    }
  } else if (ModeToggle.active == 1 && selected_notenode != NULL) {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePosition, get_note_rect(selected_notenode->note, selected_notetrack))) {
      TypeCombo[0].active = selected_notenode->note.type;
      TypeCombo[1].active = selected_notenode->note.direction;
      ModeToggle.active = notetype_to_modetoggle(selected_notenode->note.type);
      remove_note(selected_notetrack, selected_notenode->note.time, false);
      sflag = false;
    }
  } else if (ModeToggle.active > 1 && ModeToggle.active < 9) {
    if (CheckCollisionPointRec(mousePosition, scrollscore.bounds) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      int newtime = limit_num(((scrollscore.bounds.y + scrollscore.bounds.height/2 - mousePosition.y) * scrollscore.resolution * CHUNK_SIZE + cur), 0, playbar.max);
      int newtrack = limit_num(((mousePosition.x - scrollscore.bounds.x) / (scrollscore.bounds.width / 6)), 0, TRACK_NUM);
      switch(modetoggle_to_notetype(ModeToggle.active)) {
      case 0: sflag |= insert_note(newtrack, create_click_note(newtime), false); break;
      case 1: sflag |= insert_note(newtrack, create_slip_node(newtime, TypeCombo[1].active), false); break;
      case 2: sflag |= insert_note(newtrack, create_start_hold_node(newtime), false); break;
      case 3: sflag |= insert_note(newtrack, create_start_drag_node(newtime, TypeCombo[1].active), false); break;
      case 4: sflag |= insert_note(newtrack, create_passing_drag_node(newtime, TypeCombo[1].active), false); break;
      case 5: sflag |= insert_note(newtrack, create_end_node(newtime), false); break;
      case 6: sflag |= insert_note(newtrack, create_fun_node(newtime, TypeCombo[1].active), false); break;
      default: sflag |= false; break;
      }
      if (!sflag) {
	UndoCommand();
	selected_notenode = NULL;
	selected_notetrack = -1;
      } else {
	selected_notenode = get_notenode(newtrack, newtime);
	selected_notetrack = newtrack;
      }
      sflag = true;
    }
  }
}

void game_init() {

  InitWindow(screenWidth, screenHeight, title);
  InitAudioDevice();
  SetTargetFPS(60);
}

void game_update() {
  mousePosition = GetMousePosition();
  if (mouseToggle && ModeToggle.active > 1 && CheckCollisionPointRec(mousePosition, scrollscore.bounds)) mousePosition.y = scrollscore.bounds.y + scrollscore.bounds.height/2;
  if (IsKeyReleased(KEY_T)) mouseToggle ^= true;
  
  if (state & STATE_FILEOPENED) {
    cursor_update();
    playbutton_action();

    if (!(state & STATE_PLAY)) {
      if (ModeToggle.active == 9) {
#ifdef __APPLE__
#include <sys/stat.h>
	mkdir("/Users/Shared/SoundTHUMB", 0777);
	write_score_file(TextFormat("/Users/Shared/SoundTHUMB/%s.wind", DragFileWindow.text));
#elif _WIN32
	write_score_file(TextFormat("%s.wind", DragFileWindow.text));
#endif
	ModeToggle.active = 0;
	selected_notenode = NULL;
	selected_notetrack = -1;
      } else {
	editupdate();
      }
    }
  } else {
    if (state & STATE_FILELOADED) {
      if (DragFileWindow.ret == 2) {
	sound_unload();
	clear_score_list();
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
#ifdef __APPLE__
	      read_score_file(TextFormat("/Users/Shared/SoundTHUMB/%s.wind", DragFileWindow.text));
#elif _WIN32
	      read_score_file(TextFormat("%s.wind", DragFileWindow.text));
#endif
	      state |= STATE_FILELOADED;
	      TimeSpinner[0].sequencelength = get_song_length();
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

  while (!WindowShouldClose()) {
    game_update();
    game_draw();
  }
  game_exit();
  return 0;
}
