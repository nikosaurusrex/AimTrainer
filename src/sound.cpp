#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>

#include "sound.h"

struct Sound {
    ma_engine           engine;
    ma_fence            fence;
    array_t<ma_sound>   sounds;
};

/* Fucking miniaudio.h imports some shit like windows.h that has some very weird macros that destroys code
   therefore we just hide all sound stuff here
*/
static Sound *sound;

void InitializeSound() {
    sound = new Sound;

    ma_result result = ma_engine_init(0, &sound->engine);
    if (result != MA_SUCCESS) {
        LogFatal("Failed to initialize audio engine");
    }
    LogInfo("Initialized audio engine");
}

void DestroySound() {
    for (int i = 0; i < sound->sounds.size(); i++) {
        ma_sound_uninit(&sound->sounds[i]);
    }

    ma_engine_uninit(&sound->engine);

    delete sound;
}

void PlaySoundById(u32 sound_id) {
    ma_sound_start(&sound->sounds[sound_id]);
}

u32 LoadSound(const char *path) {
    u32 id = sound->sounds.size();
    
    sound->sounds.push_back(ma_sound());

    ma_sound *sound_data = &sound->sounds[id];
    ma_result result = ma_sound_init_from_file(&sound->engine, path, MA_SOUND_FLAG_NO_SPATIALIZATION, 0, 0, sound_data);

    if (result != MA_SUCCESS) {
        LogFatal("Failed to load sound '%s'", path);
    }

    return id;
}