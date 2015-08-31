#include <SDL2/SDL.h>
#include "sdl-audio.h"

//
// start everything up
//
bool SDLAudio::init()
{
	SDL_AudioSpec wanted, have;
    //extern void fill_audio(void *udata, Uint8 *stream, int len);

    /* Set the audio format */
    wanted.freq = 22050;
    wanted.format = AUDIO_S16; //AUDIO_F32
    wanted.channels = 2;    /* 1 = mono, 2 = stereo */
    wanted.samples = 1024;  /* Good low-latency value for callback */
    wanted.callback = mix_audio;
    wanted.userdata = NULL;

    /* Open the audio device, forcing the desired format */
    /*if(SDL_OpenAudio(&wanted, NULL) < 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return false;
    }*/

/*    int i, count = SDL_GetNumAudioDevices(0);
    for(i = 0; i < count; i++)
    {
        SDL_Log("Audio device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
    }

    const char *device_name = SDL_GetAudioDeviceName(0, 0);*/
    SDL_OpenAudioDevice(NULL, 0, &wanted, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	SDL_PauseAudio(0);

	return true;
}

//
// shut everything down
//
void SDLAudio::shut_down()
{
	/* Wait for sound to complete */
    while(audio_len > 0) {
        SDL_Delay(100);
    }
    SDL_CloseAudio();
}


void SDLAudio::play_sound(const char *file)
{
	int index;
    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dlen;
    SDL_AudioCVT cvt;

    /* Look for an empty (or finished) sound slot */
    for(index=0; index < NUM_SOUNDS; ++index) {
        if(samples_playing[index].dpos == samples_playing[index].dlen) {
            break;
        }
    }

    if(index == NUM_SOUNDS)
    {
        return;
    }

    /* Load the sound file and convert it to 16-bit stereo at 22kHz */
    if(SDL_LoadWAV(file, &wave, &data, &dlen) == NULL )
    {
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        return;
    }

    SDL_BuildAudioCVT(&cvt,
                      wave.format,
                      wave.channels,
                      wave.freq,
                      AUDIO_S16,
                      2,
                      22050);

    cvt.buf = (Uint8 *)malloc(dlen*cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;
    SDL_ConvertAudio(&cvt);
    SDL_FreeWAV(data);

    /* Put the sound data in the slot (it starts playing immediately) */
    if(samples_playing[index].data ) {
        free(samples_playing[index].data);
    }
    SDL_LockAudio();
    samples_playing[index].data = cvt.buf;
    samples_playing[index].dlen = cvt.len_cvt;
    samples_playing[index].dpos = 0;
    SDL_UnlockAudio();
}

void SDLAudio::mix_audio(void *unused, Uint8 *stream, int len)
{
    Uint32 amount;

    for(int i = 0; i < NUM_SOUNDS; i++) {
        amount = (samples_playing[i].dlen - samples_playing[i].dpos);
        if(amount > (Uint32)len) {
            amount = len;
        }
        SDL_MixAudio(stream, &samples_playing[i].data[samples_playing[i].dpos], amount, SDL_MIX_MAXVOLUME);
        samples_playing[i].dpos += amount;
    }
}
