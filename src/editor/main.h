#ifndef __EDITOR_MAIN_H__
#define __EDITOR_MAIN_H__

#include "../scoreio/score.h"
#include "../audio/sound.h"
#include "../others/raylib.h"
#include "../others/utils.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "../others/raygui.h"

#define screenWidth      800
#define screenHeight     600
#define title            "Sound-THUMB AFD | Score Editor"

#define STATE_FILELOADED        (1 << 0)
#define STATE_FILEOPENED        (1 << 1)
#define STATE_PLAY              (1 << 2)

#define jumpDistance 5000

unsigned char state = 0;
Vector2 mousePosition;
int cur;


#endif
