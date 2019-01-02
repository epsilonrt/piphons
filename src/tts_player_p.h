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
#ifndef PIPHONS_TTS_PLAYER_PRIVATE_H
#define PIPHONS_TTS_PLAYER_PRIVATE_H

#include <alsa/asoundlib.h>
#include <piphons/tts.h>

namespace Piphons {

  class Tts::Player::Private {
    public:
      Private (Player * q);
      ~Private();

      Player * const q_ptr;
      snd_pcm_t * handle;
      int mode; // 0 = blocking; SND_PCM_NONBLOCK = not blocking
      std::string device;
      std::string error;

      PIMP_DECLARE_PUBLIC (Player)
  };
}
/* ========================================================================== */
#endif /* PIPHONS_TTS_PLAYER_PRIVATE_H defined */
