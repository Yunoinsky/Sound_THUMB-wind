#ifndef __UTILS_H__
#define __UTILS_H__

#include "raylib.h"

#define limit_value(__v, __min_v, __max_v)	\
  if (__v < __min_v) __v = __min_v;		\
  else if (__v > __max_v) __v = __max_v;

inline bool loop_acc(int *timer, int step, int limit) {
  *timer += step;
  if (*timer > limit) {
    *timer = 0;
    return true;
  }
  return false;
}


#endif
