#ifndef SDLW_AUDIO_HPP
#define SDLW_AUDIO_HPP

#include <boost/intrusive_ptr.hpp>

#include <string>

struct sdlw_sound_data
{
    sdlw_sound_data() :
      m_buffer(0),
      m_length(0),
      m_channels(0),
      m_refc(0)
    {
    }

    short * m_buffer;
    int     m_length;
    int     m_channels;
    int     m_refc;
};

void intrusive_ptr_add_ref(sdlw_sound_data *);
void intrusive_ptr_release(sdlw_sound_data *);

class sdlw_sound
{
  public:

    // Takes ownership of a malloc'd buffer
    sdlw_sound(short * buffer, int samples, int channels);

    const sdlw_sound_data * data() const;

  private:

    boost::intrusive_ptr<sdlw_sound_data> m_data;
};

void sdlw_start_audio();
void sdlw_stop_audio();

sdlw_sound load_sample(std::string filename);
void play_sample(const sdlw_sound &);

#endif
