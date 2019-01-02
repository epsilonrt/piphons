/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
 * This file is part of the libpiphons Library.
 *
 * The libpiphons Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libpiphons Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libpiphons Library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <exception>
#include <piduino/system.h>
#include "tts_player_p.h"

namespace Piphons {

  // ---------------------------------------------------------------------------
  //
  //                      Tts::Player::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Tts::Player::Private::Private (Tts::Player * q) :
    q_ptr (q),
    handle (0),
    mode (0) { }

  // ---------------------------------------------------------------------------
  Tts::Player::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  //
  //                        Tts::Player Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Tts::Player::Player (Tts::Player::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Tts::Player::Player () :
    d_ptr (new Private (this)) {}

  // ---------------------------------------------------------------------------
  Tts::Player::~Player() {

    close();
  }

  // ---------------------------------------------------------------------------
  const std::string & Tts::Player::errorString() const {
    PIMP_D (const Tts::Player);

    return d->error;
  }
  
  // ---------------------------------------------------------------------------
  const std::string & Tts::Player::device() const {
    PIMP_D (const Tts::Player);

    return d->device;
  }

  // ---------------------------------------------------------------------------
  bool Tts::Player::open (const std::string & dev) {

    if (!isOpen()) {
      PIMP_D (Tts::Player);
      int err;

      if ( (err = snd_pcm_open (&d->handle, dev.c_str(), SND_PCM_STREAM_PLAYBACK, d->mode)) < 0) {

        d->error = snd_strerror (err);
      }

      else if ( (err = snd_pcm_set_params (d->handle,
                                           SND_PCM_FORMAT_S16_LE,
                                           SND_PCM_ACCESS_RW_INTERLEAVED,
                                           1,
                                           16000,
                                           1,
                                           200000)) < 0) {      /* 0.2 sec */
        d->error = snd_strerror (err);
        close();
      }
      else {

        d->device = dev;
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void Tts::Player::close() {

    if (isOpen()) {
      PIMP_D (Tts::Player);

      snd_pcm_close (d->handle);
      d->handle = 0;
      d->device.clear();
    }
  }

  // ---------------------------------------------------------------------------
  bool Tts::Player::isOpen() const {
    PIMP_D (const Tts::Player);

    return d->handle != 0;
  }

  // ---------------------------------------------------------------------------
  bool Tts::Player::write (const void * buffer, unsigned long size) {
    bool success = isOpen();
    
    if (success) {
      PIMP_D (Tts::Player);

      snd_pcm_sframes_t frames = snd_pcm_writei (d->handle, buffer, size / 2);
      success = (frames >= 0);

      if (! success) {

        frames = snd_pcm_recover (d->handle, frames, 0);
        success = (frames >= 0);
        if (! success) {
          d->error = snd_strerror (frames);
        }
      }
    }
    return success;
  }
}
/* ========================================================================== */
