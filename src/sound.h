#ifndef SOUND_H
#define SOUND_H

#include "common.h"

void InitializeSound();
void DestroySound();

void PlaySoundById(u32 sound_id);
u32 LoadSound(const char *path);

#endif