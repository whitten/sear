// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

#include "Sound.h"
#include "Console.h"
#include "common/Log.h"
#include <SDL/SDL.h>
#include <unistd.h>
#include <cassert>

// $Id: Sound.cpp,v 1.21 2007-05-02 20:47:55 simon Exp $

// TODO: The sound systems appear to have a large number of memory leaks in SDL and/or SDL_mixer

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

  static const std::string PLAY_SOUND = "play_sound";
  static const std::string PLAY_SOUND_LOOP = "play_sound_loop";
  static const std::string STOP_SOUND_LOOP = "stop_sound_loop";
  static const std::string PLAY_MUSIC = "play_music";
  static const std::string STOP_MUSIC = "stop_music";
  static const std::string ENABLE_SOUND = "enable_sound";
  
Sound::Sound() :
  m_initialised(false)
{}

Sound::~Sound() {
 if (m_initialised) shutdown();
}
	
int Sound::init() {
  assert(m_initialised == false);
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    Log::writeLog(std::string("Error init SDL_AUDIO: ") + SDL_GetError(), Log::LOG_ERROR);
    return 1;
  }
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
    Log::writeLog(std::string("Error init SDL_mixer: ") + Mix_GetError(), Log::LOG_ERROR);
    return 1;
  }
  m_initialised = true;
  return 0;
}

void Sound::shutdown() {
  assert(m_initialised == true);
  Mix_CloseAudio();
  while(!m_sound_map.empty()) {
    Mix_Chunk *sample = (m_sound_map.begin())->second;
    if (sample) Mix_FreeChunk(sample);
    m_sound_map.erase(m_sound_map.begin());
  }
  while(!m_music_map.empty()) {
    Mix_Music *music = (m_music_map.begin())->second;
    if (music) Mix_FreeMusic(music);
    m_music_map.erase(m_music_map.begin());
  }
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
  m_initialised = false;
}

Mix_Chunk *Sound::getSample(const std::string &file_name) {
  Mix_Chunk *sample = NULL;
  sample = m_sound_map[file_name];
  if (sample) return sample;
  sample=Mix_LoadWAV(file_name.c_str());
  if (sample) {
    m_sound_map[file_name] = sample;
    return sample;
  }
  Log::writeLog(std::string("Mix_LoadWAV: ") + Mix_GetError(), Log::LOG_ERROR);
  return NULL;
}

Mix_Music *Sound::getMusic(const std::string &file_name) {
  Mix_Music *music = NULL;
  music = m_music_map[file_name];
  if (music) return music;
  music=Mix_LoadMUS(file_name.c_str());
  if (music) {
    m_music_map[file_name] = music;
    return music;
  }
  Log::writeLog(std::string("Mix_LoadMUS: ") + Mix_GetError(), Log::LOG_ERROR);
  return NULL;
}

void Sound::playSound(const std::string &file_name) {
  // load sample.wav in to sample
 Mix_Chunk *sample = getSample(file_name);
 Mix_PlayChannel(1, sample, 0);
}

void Sound::playSoundLoop(const std::string &file_name) {
  // load sample.wav in to sample
 Mix_Chunk *sample = getSample(file_name);
 Mix_PlayChannel(2, sample, -1);
}

void Sound::stopSoundLoop() {
  Mix_HaltChannel(2);
}

void Sound::playMusic(const std::string &file_name) {
  // load sample.wav in to sample
 Mix_Music *music = getMusic(file_name);
 Mix_PlayMusic(music, 0);
}

void Sound::stopMusic() {
  Mix_FadeOutMusic(100);
  Mix_RewindMusic();
  
}
void Sound::registerCommands(Console *console) {
  console->registerCommand(PLAY_SOUND, this);
  console->registerCommand(PLAY_SOUND_LOOP, this);
  console->registerCommand(STOP_SOUND_LOOP, this);
  console->registerCommand(PLAY_MUSIC, this);
  console->registerCommand(STOP_MUSIC, this);
  console->registerCommand(ENABLE_SOUND, this);
}

void Sound::runCommand(const std::string &command, const std::string &args) {
  if (command == PLAY_SOUND) playSound(args);
  else if (command == PLAY_SOUND_LOOP) playSoundLoop(args);
  else if (command == STOP_SOUND_LOOP) stopSoundLoop();
  else if (command == PLAY_MUSIC) playMusic(args);
  else if (command == STOP_MUSIC) stopMusic();
  else if (command == ENABLE_SOUND)
  {
    //setEnabled(args == "1");
  }
}

} /* namespace Sear */
