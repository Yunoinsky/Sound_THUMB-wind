#include "score.h"

void write_score_file(const char* filename){
  FILE* fp = fopen(filename, "wb");
  fwrite(head, sizeof(char), 5, fp);
  if (fp) {
    NoteNode* cur;
    for (unsigned char i = 0; i < TRACK_NUM; i++) {
      cur = Notelists[i]->next;
      while(cur) {
	fwrite(&i, sizeof(unsigned char), 1, fp);
	fwrite(&(cur->note), sizeof(Note), 1, fp);
	cur = cur->next;
      }
    }
    fclose(fp);
  }
}

void read_score_file(const char* filename) {
  if (!Notelists[0]) {
    init_score_list();
  }
  FILE* fp = fopen(filename, "rb");
  if (fp) {
    char headtmp[5];
    int flag;
    unsigned char tempint;
    Note tempnote;
    fread(headtmp, sizeof(char), 5, fp);
    if ((headtmp[0] == 'T') &&
	(headtmp[1] == 'H') &&
	(headtmp[2] == 'U') &&
	(headtmp[3] == 'M') &&
	(headtmp[4] == 'B')) {

      for(;;){

	fread(&tempint, sizeof(unsigned char), 1, fp);
	flag = fread(&tempnote, sizeof(Note), 1, fp);
	if (flag){
	  insert_note(tempint, tempnote, true); // We don't want load process in the history.
	}else
	  break;
      }
    }
    fclose(fp);
  }
}

void DoneCommand(unsigned char track, signed char command_type, Note note) {
  historystackdepth++;
  HistoryCommandNode* command = (HistoryCommandNode*)malloc(sizeof(HistoryCommandNode));
  command->note = note;
  command->type = command_type;
  command->track = track;
  command->prev = historylist;
  command->next = historylist->next;
  if (historylist->next) historylist->next->prev = command;
  else historytail = command;
  historylist->next = command;

  if (historystackdepth > HISTORYDEPTH) {
    historytail = historytail->prev;
    free(historytail->next);
    historytail->next = NULL;
  }
}

void UndoCommand() {
  if (historystackdepth > 0) {
    historystackdepth--;
    if (historylist->next) {
      if (historylist->next->type == INSERT_COMMAND) {
	remove_note(historylist->next->track, historylist->next->note.time, true);
      } else if(historylist->next->type == REMOVE_COMMAND){
	insert_note(historylist->next->track, historylist->next->note, true);
      }
      HistoryCommandNode* temp = historylist->next;
      historylist->next = historylist->next->next;
      if (historylist->next) historylist->next->prev = historylist;
    }
  }
}

void init_score_list() {
  for (unsigned int i = 0; i < TRACK_NUM; i++) {
    Notelists[i] = (NoteNode*)malloc(sizeof(NoteNode));
    Notelists[i]->note = create_fun_node(0, NOTE_DISABLE);
    Notelists[i]->next = NULL;
  }
  historylist = (HistoryCommandNode*)malloc(sizeof(HistoryCommandNode));
  
}

Note create_click_note(unsigned int time) {
  return (Note){ CLK_NOTE, time, NOTE_MEDIUM };
}

Note create_slip_node(unsigned int time, signed char direction) {
  return (Note){ SLP_NOTE, time, direction };
}

Note create_start_hold_node(unsigned int time, signed char direction) {
  return (Note){ SHD_NOTE, time, direction };
}

Note create_start_drag_node(unsigned int time, signed char direction) {
  return (Note){ SDG_NOTE, time, direction };
}

Note create_passing_drag_node(unsigned int time, signed char direction) {
  return (Note){ PDG_NOTE, time, direction };
}

Note create_end_node(unsigned int time) {
  return (Note){ END_NOTE, time, NOTE_MEDIUM };
}

Note create_fun_node(unsigned int time, signed char fun) {
  return (Note){ FUN_NODE, time, fun };
}

NoteNode* create_note_node(Note note, NoteNode* next) {
  NoteNode* notenode = (NoteNode*)malloc(sizeof(NoteNode));
  notenode->note = note;
  notenode->next = next;
  return notenode;
}

bool insert_note(unsigned char track, Note note, bool inundo) {
  NoteNode* cur = Notelists[track];
  while((cur->next != NULL) && (cur->next->note.time < note.time)) {
    cur = cur->next;
  }
  if (((note.time - cur->note.time) > MIN_NOTE_DIST) &&
      ((cur->next == NULL) || ((cur->next->note.time-note.time) > MIN_NOTE_DIST))) {
    cur->next = create_note_node(note, cur->next);
    if (!inundo) DoneCommand(track, INSERT_COMMAND, note);
    return true;
  }
  return false;
}

NoteNode* get_notenode(unsigned char track, unsigned int time) {
  NoteNode* cur = Notelists[track];
  while(cur->next != NULL) {
    cur = cur->next;
    if (cur->note.time == time) break;
  }
  return cur;
}

bool remove_note(unsigned char track, unsigned int time, bool inundo) {
  NoteNode* cur = Notelists[track];
  while(cur->next != NULL) {
    if (cur->next->note.time == time) break;
    else cur = cur->next;
  }
  if (cur->next == NULL) return false;
  else {
    if (!inundo) DoneCommand(track, REMOVE_COMMAND, cur->next->note);
    NoteNode* temp = cur->next;
    cur->next = cur->next->next;
    free(temp);
  }
}

void print_score_list() {
  printf("===========================================\n");
  NoteNode * cur;
  for (unsigned int i = 0; i < TRACK_NUM; i++) {
    cur = Notelists[i];
    while(cur) {
      printf("track: %d, time: %6d, type: %2d, direct: %d\n", i, cur->note.time, cur->note.type, cur->note.direction);
      cur = cur->next;
    }
  }
}


#ifdef TEST
inline void text1() {
  NoteNode *cur = Notelists[1];

  for (int i = 1; i < 81; i++) {
    insert_note(1, create_click_note(100*i), false);
  }
  UndoCommand();
  insert_note(1, create_click_note(900), false);
  write_score_file("a.txt");
}

inline void text2() {
  read_score_file("a.txt");
}

int main(int argc, char ** argv) {
  init_score_list();
  text2();
  print_score_list();
  return 0;
}
#endif
