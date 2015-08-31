#ifndef _SDL_AUDIO_H_
#define _SDL_AUDIO_H_

#define NUM_SOUNDS 1
#define AUDIO_TEST_FILE "data/audio/test/ocean.wav"

namespace SDLAudio
{
	typedef struct {
		Uint8 *data;
		Uint32 dpos;
		Uint32 dlen;
	} Sample;

	bool init();
	void shut_down();

	void play_sound(const char *file);

	void process(float t);
	static void mix_audio(void *udata, Uint8 *stream, int len);

	static Sample samples_playing[NUM_SOUNDS];
	//static Uint8 *audio_chunk;
    static Uint32 audio_len;
    //static Uint8 *audio_pos;
}

#endif // _SDL_AUDIO_H_
