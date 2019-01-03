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
  void * Tts::Private::parser (const std::string & voiceIso, const std::string & device,
                               std::future<void> run, Private * d) {
    std::string text;
    bool success = true;

    if (d->engine.open (voiceIso, device)) {

      while (d->engine.isOpen() && success) {

        if (! d->fifo.read (text, 500)) {
          // nothing to say 
          
          if (run.wait_for (std::chrono::milliseconds (1)) != std::future_status::timeout) {
            
            break;
          }
          text.clear(); // set mute text
        }
        
        success = d->engine.write (text);
        if (success) {
          
          success = d->engine.play();
        }
      }
      d->engine.close();
    }
    return 0;
  }

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

      // Fetch std::future object associated with promise
      std::future<void> running = d->stopParsing.get_future();

      // Starting Thread & move the future object in lambda function by reference
      d->parseThread = std::thread (&Private::parser, voiceIso, device, std::move (running), d);
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void Tts::close() {

    if (isOpen()) {
      PIMP_D (Tts);

      // Set the value in promise
      d->stopParsing.set_value();
      // Wait for thread to join
      d->parseThread.join();
    }
  }

  // ---------------------------------------------------------------------------
  bool Tts::isOpen() const {
    PIMP_D (const Tts);

    return d->parseThread.joinable();
  }

  // ---------------------------------------------------------------------------
  void Tts::write (const std::string & text, int volume, int speed, int pitch) {
    PIMP_D (Tts);

    d->text = Engine::formatText (text, volume, speed, pitch);
  }

  // ---------------------------------------------------------------------------
  void Tts::append (const std::string & text, int volume, int speed, int pitch) {
    PIMP_D (Tts);

    d->text += Engine::formatText (text, volume, speed, pitch);
  }

  // ---------------------------------------------------------------------------
  bool Tts::say () {
    PIMP_D (Tts);

    if (isOpen()) {

      d->fifo.write (d->text);
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Tts::say (const std::string & text, int volume, int speed, int pitch) {

    write (text, volume, speed, pitch);
    return say();
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
