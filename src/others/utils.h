#ifndef __UTILS_H__
#define __UTILS_H__

#include "raylib.h"

#define limit_value(__v, __min_v, __max_v)	\
  if (__v < __min_v) __v = __min_v;		\
  else if (__v > __max_v) __v = __max_v;

#define limit_num(__v, __min_v, __max_v)        \
  ((__v < __min_v)? __min_v:((__v > __max_v)?__max_v:__v))

#define yumax(__a,__b) (__a > __b ? __a : __b)

#define yumin(__a,__b) (__a < __b ? __a : __b)



inline bool loop_acc(int *timer, int step, int limit) {
  *timer += step;
  if (*timer > limit) {
    *timer = 0;
    return true;
  }
  return false;
}


#endif
