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
#include "tts_p.h"

namespace Piphons {

  // ---------------------------------------------------------------------------
  //
  //                            Tts::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Tts::Private::Private (Tts * q) :
    q_ptr (q) {

  }

  // ---------------------------------------------------------------------------
  Tts::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  //
  //                                Tts Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Tts::Tts (Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Tts::Tts () : d_ptr (new Private (this)) {}

  // ---------------------------------------------------------------------------
  Tts::~Tts() = default;

  // ---------------------------------------------------------------------------
  bool Tts::open (const std::string & voiceIso, const std::string & device) {

    if (!isOpen()) {
      PIMP_D (Tts);

      return d->engine.open (voiceIso, device);
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  void Tts::close() {

    if (isOpen()) {
      PIMP_D (Tts);

      d->engine.close();
    }
  }

  // ---------------------------------------------------------------------------
  bool Tts::isOpen() const {
    PIMP_D (const Tts);

    return d->engine.isOpen();
  }

  // ---------------------------------------------------------------------------
  bool Tts::write (const std::string & text, int volume, int speed, int pitch) {
    PIMP_D (Tts);

    return d->engine.write (text, volume, speed, pitch);
  }

  // ---------------------------------------------------------------------------
  bool Tts::append (const std::string & text, int volume, int speed, int pitch) {
    PIMP_D (Tts);

    return d->engine.append (text, volume, speed, pitch);
  }

  // ---------------------------------------------------------------------------
  bool Tts::say () {
    PIMP_D (Tts);

    return d->engine.play();
  }

  // ---------------------------------------------------------------------------
  bool Tts::say (const std::string & text, int volume, int speed, int pitch) {

    if (write (text, volume, speed, pitch)) {
      
      return say();
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  const Tts::Voice & Tts::voice() const {
    PIMP_D (const Tts);

    return d->engine.voice();
  }

  // ---------------------------------------------------------------------------
  const std::string & Tts::errorString() const {
    PIMP_D (const Tts);

    return d->engine.errorString();
  }

  // ---------------------------------------------------------------------------
  const std::string & Tts::device() const {
    PIMP_D (const Tts);

    return d->engine.device();
  }

}
/* ========================================================================== */
