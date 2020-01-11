#ifndef __SCORE_H__
#define __SCORE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define TRACK_NUM 6

typedef struct Note {
  enum NoteType {
		 CLK_NOTE,     // click note
		 SLP_NOTE,     // slip note
		 SHD_NOTE,     // start hold note

		 SDG_NOTE,     // start drag note
		 PDG_NOTE,     // passing drag note

		 END_NOTE,     // end for hold or drag
		
		 FUN_NODE  } type;   // function note
  unsigned int time;
  signed char direction;
} Note;

typedef struct NoteNode {
  Note note;
  struct NoteNode* next;
} NoteNode;

static NoteNode* Notelists[TRACK_NUM] = { 0 };

typedef struct HistoryCommandNode {
  enum CommandType {
		    INSERT_COMMAND,
		    REMOVE_COMMAND  } type; // insert_command means that you need to remove it if you want to undo the command
  
  unsigned char track;
  Note note;
  struct HistoryCommandNode* prev;
  struct HistoryCommandNode* next;
} HistoryCommandNode;

#define HISTORYDEPTH 50
static unsigned short historystackdepth  = 0; // The depth of history stack must be limited
HistoryCommandNode *historylist;
HistoryCommandNode *historytail;

void DoneCommand(unsigned char track, signed char command_type, Note note);
void UndoCommand();


#define NOTE_LEFT       -1
#define NOTE_MEDIUM      0
#define NOTE_RIGHT       1

#define NOTE_DISABLE     0
#define NOTE_ENABLE      1
#define NOTE_CADENZA     2

static const char head[5] = "THUMB";
void write_score_file(const char* filename);

void read_score_file(const char* filename);

void DoneCommand(unsigned char track, signed char command_type, Note note);

void Undocommand();

void init_score_list();

void clear_score_list();

Note create_click_note(unsigned int time);

Note create_slip_node(unsigned int time, signed char direction);

Note create_start_hold_node(unsigned int time, signed char direction);

Note create_start_drag_node(unsigned int time, signed char direction);

Note create_passing_drag_node(unsigned int time, signed char direction);

Note create_end_node(unsigned int time);

Note create_fun_node(unsigned int time, signed char fun);

NoteNode* create_note_node(Note note, NoteNode* next);

#define MIN_NOTE_DIST 20
bool insert_note(unsigned char track, Note note, bool inundo);

Note* get_note(unsigned char track, unsigned int time);

bool remove_note(unsigned char track, unsigned int time, bool inundo);


#endif
