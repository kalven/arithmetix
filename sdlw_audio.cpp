#include "sdlw_audio.hpp"
#include "resource.hpp"

#include "stb_vorbis_ext.h"
#include <SDL/SDL.h>

#include <algorithm>
#include <iostream>
#include <cstring>

const int max_streams = 4;

struct audio_stream
{
    const sdlw_sound_data * m_sample;
    int                     m_index;
};

static bool          audio_started;
static SDL_AudioSpec audio_spec;
static audio_stream  audio_streams[max_streams];

static void mixer_cb(void *, Uint8 * outbuf, int outlen)
{
    outlen /= 2;

    for(int i = 0; i != max_streams; ++i)
    {
        audio_stream * as = &audio_streams[i];
        if(const sdlw_sound_data * sd = as->m_sample)
        {
            int samples = std::min(outlen, sd->m_length - as->m_index);
            short * buf = (short*)outbuf;
            for(int j = 0; j != samples; ++j)
            {
                buf[j] += sd->m_buffer[as->m_index+j];
            }
            //            SDL_MixAudio(outbuf, (Uint8*)(sd->m_buffer + as->m_index), samples*2, SDL_MIX_MAXVOLUME);

            as->m_index += samples;
            if(as->m_index == sd->m_length)
            {
                as->m_index = 0;
                as->m_sample = 0;
            }
        }
    }
}

void sdlw_start_audio()
{
    assert(!audio_started);

    SDL_AudioSpec wanted = {};

    wanted.freq     = 44100;
    wanted.channels = 2;
    wanted.format   = AUDIO_U16SYS;
    wanted.samples  = 2048;
    wanted.callback = &mixer_cb;

    int res = SDL_OpenAudio(&wanted, &audio_spec);
    if(res == -1)
    {
        std::cout << "Audio init failed." << std::endl;
        return;
    }

    SDL_PauseAudio(0);
    audio_started = true;
}

void sdlw_stop_audio()
{
    assert(audio_started);
    SDL_CloseAudio();
}

sdlw_sound::sdlw_sound(short * buffer, int samples, int channels) :
  m_data(new sdlw_sound_data)
{
    m_data->m_buffer = buffer;
    m_data->m_length = samples;
    m_data->m_channels = channels;
}

const sdlw_sound_data * sdlw_sound::data() const
{
    return m_data.get();
}

void intrusive_ptr_add_ref(sdlw_sound_data * data)
{
    data->m_refc++;
}

void intrusive_ptr_release(sdlw_sound_data * data)
{
    if(--data->m_refc == 0)
    {
        if(data->m_buffer)
            std::free(data->m_buffer);
        delete data;
    }
}

sdlw_sound load_sample(std::string filename)
{
    int vorbis_length;
    void * vorbis_data = get_resource_mem(filename.c_str(), &vorbis_length);

    short * pcm_buffer;
    int pcm_channels;
    int pcm_samples = stb_vorbis_decode_memory((unsigned char*)vorbis_data, vorbis_length, &pcm_channels, &pcm_buffer);
    // TODO: Add error checking

    return sdlw_sound(pcm_buffer, pcm_samples * 2, pcm_channels);
}

void play_sample(const sdlw_sound & snd)
{
    if(!audio_started)
        return;
    static int next_stream = 0;
    SDL_LockAudio();
    audio_streams[next_stream].m_sample = snd.data();
    audio_streams[next_stream++].m_index = 0;
    if(next_stream == max_streams)
        next_stream = 0;
    SDL_UnlockAudio();
}
