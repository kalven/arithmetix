#ifndef ARITHMETIX_STATE_HPP
#define ARITHMETIX_STATE_HPP

#include "sdlw.hpp"
#include "transition.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <string>
#include <map>

class state : boost::noncopyable
{
  public:
    virtual ~state()
    {
    }

    virtual void activate_state(const state_arg &)
    {
    }

    virtual void deactivate_state()
    {
    }

    virtual void on_event(const SDL_Event &) = 0;
    virtual void on_draw(sdl_surface &) = 0;
};

class state_machine
{
  public:
    state_machine()
    {
        m_current = m_next = m_states.end();
    }

    void add_state(const std::string & name, boost::shared_ptr<state> s)
    {
        m_states[name] = s;
    }

    void set_next_state(const std::string & name)
    {
        set_next_state(name, (void*)0);
    }

    void set_next_state(const std::string & name, const state_arg & args)
    {
        // TODO: Verify that the state exists.
        m_next = m_states.find(name);
        m_next_args = args;

        // If there's no active state, activate this right away
        if(m_current == m_states.end())
        {
            m_current = m_next;
            m_current->second->activate_state(m_next_args);
        }
    }

    void send_event(const SDL_Event & e)
    {
        if(m_current != m_states.end())
            m_current->second->on_event(e);
        if(m_current != m_next && m_next != m_states.end())
        {
            m_current->second->deactivate_state();
            m_current = m_next;
            m_current->second->activate_state(m_next_args);
        }
    }

    void draw(sdl_surface & screen)
    {
        if(m_current != m_states.end())
            m_current->second->on_draw(screen);
    }

  private:

    typedef std::map<std::string, boost::shared_ptr<state> > state_map;

    state_map m_states;
    state_map::iterator m_current;
    state_map::iterator m_next;
    state_arg m_next_args;
};

#endif
