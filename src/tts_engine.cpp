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
#include <cstring>
#include <exception>
#include <sstream>
#include <piduino/system.h>
#include "tts_engine_p.h"

namespace Piphons {

  // ---------------------------------------------------------------------------
  //
  //                      Tts::Engine::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // static
  const pico_Char Tts::Engine::Private::voiceName[] = "TtsEngineVoice";

  // ---------------------------------------------------------------------------
  Tts::Engine::Private::Private (Tts::Engine * q) :
    q_ptr (q),
    voice (Voice::voice ("en-US")),
    system (0),
    engine (0),
    memory (0),
    taResource (0),
    sgResource (0),
    taResourceName (0),
    sgResourceName (0)  {

  }

  // ---------------------------------------------------------------------------
  Tts::Engine::Private::~Private() {

    free (memory);
    free (taResourceName);
    free (sgResourceName);
  }

  // ---------------------------------------------------------------------------
  bool Tts::Engine::Private::open() {
    int ret;
    pico_Retstring str;

    memory = malloc (MemorySize);
    if ( (ret = pico_initialize (memory, MemorySize, &system)) != PICO_OK) {

      if (system != 0) {

        pico_getSystemStatusMessage (system, ret, str);
        error.assign (str);
        pico_terminate (&system);
        system = 0;
      }
      return false;
    }

    // Load the text analysis Lingware resource file
    if ( (ret =
            pico_loadResource (
              system,
              (const pico_Char *) voice->taFilePath().c_str(),
              &taResource)) != PICO_OK) {

      pico_getSystemStatusMessage (system, ret, str);
      error.assign (str);
      goto unloadTaResource;
    }

    // Load the signal generation Lingware resource file
    if ( (ret =
            pico_loadResource (
              system,
              (const pico_Char *) voice->sgFilePath().c_str(),
              &sgResource)) != PICO_OK) {

      pico_getSystemStatusMessage (system, ret, str);
      error.assign (str);
      goto unloadSgResource;
    }

    // Get the text analysis resource name
    taResourceName = (pico_Char *) malloc (PICO_MAX_RESOURCE_NAME_SIZE);
    if ( (ret =
            pico_getResourceName (
              system,
              taResource,
              (char *) taResourceName)) != PICO_OK) {

      pico_getSystemStatusMessage (system, ret, str);
      error.assign (str);
      goto unloadSgResource;
    }

    // Get the signal generation resource name
    sgResourceName = (pico_Char *) malloc (PICO_MAX_RESOURCE_NAME_SIZE);
    if ( (ret =
            pico_getResourceName (
              system,
              sgResource,
              (char *) sgResourceName)) != PICO_OK) {

      pico_getSystemStatusMessage (system, ret, str);
      error.assign (str);
      goto unloadSgResource;
    }

    // Create a voice definition
    if ( (ret = pico_createVoiceDefinition (system, voiceName)) != PICO_OK) {

      pico_getSystemStatusMessage (system, ret, str);
      error.assign (str);
      goto unloadSgResource;
    }

    // Add the text analysis resource to the voice
    if ( (ret =
            pico_addResourceToVoiceDefinition (
              system,
              voiceName,
              taResourceName)) != PICO_OK) {

      pico_getSystemStatusMessage (system, ret, str);
      error.assign (str);
      goto unloadSgResource;
    }

    // Add the signal generation resource to the voice
    if ( (ret =
            pico_addResourceToVoiceDefinition (
              system,
              voiceName,
              sgResourceName)) != PICO_OK) {

      pico_getSystemStatusMessage (system, ret, str);
      error.assign (str);
      goto unloadSgResource;
    }

    // Create a new Pico engine
    if ( (ret = pico_newEngine (system, voiceName, &engine)) != PICO_OK) {

      pico_getSystemStatusMessage (system, ret, str);
      error.assign (str);
      goto disposeEngine;
    }

    // success
    return true;

    //--------------------------------------------------------------------------
    // partial shutdowns below this line
    // for pico cleanup in case of startup abort
disposeEngine:
    if (engine) {

      pico_disposeEngine (system, &engine);
      pico_releaseVoiceDefinition (system, voiceName);
      engine = 0;
    }
unloadSgResource:
    if (sgResource) {

      pico_unloadResource (system, &sgResource);
      sgResource = 0;
    }
unloadTaResource:
    if (taResource) {

      pico_unloadResource (system, &taResource);
      taResource = 0;
    }
    if (system) {

      pico_terminate (&system);
      system = 0;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void Tts::Engine::Private::close() {

    if (engine) {

      pico_disposeEngine (system, &engine);
      pico_releaseVoiceDefinition (system, voiceName);
      engine = 0;
    }

    if (sgResource) {

      pico_unloadResource (system, &sgResource);
      sgResource = 0;
    }

    if (taResource) {

      pico_unloadResource (system, &taResource);
      taResource = 0;
    }

    if (system) {

      pico_terminate (&system);
      system = 0;
    }
  }

  // ---------------------------------------------------------------------------
  int Tts::Engine::Private::process() {
    int ret, getstatus;
    pico_Int16  bytes_sent, bytes_recv, text_remaining, out_data_type;
    uint8_t buffer[BufferSize];
    uint16_t samples[BufferSize / 2];
    pico_Char * p;
    pico_Retstring str;
    size_t bufferlen = 0;

    p = (pico_Char *) &text2speech[0];
    text_remaining = text2speech.size() + 1;

    // synthesis loop
    while (text_remaining) {

      // Feed the text into the engine
      if ( (ret = pico_putTextUtf8 (engine, p, text_remaining, &bytes_sent))) {

        pico_getSystemStatusMessage (system, ret, str);
        error.assign (str);
        return -1;
      }

      text_remaining -= bytes_sent;
      p += bytes_sent;

      do {

        // Retrieves the next samples
        getstatus = pico_getData (engine, (void *) samples,
                                  BufferSize / 2, &bytes_recv, &out_data_type);

        if ( (getstatus != PICO_STEP_BUSY) && (getstatus != PICO_STEP_IDLE)) {

          pico_getSystemStatusMessage (system, getstatus, str);
          error.assign (str);
          return -1;
        }

        if (bytes_recv) {

          if ( (bufferlen + bytes_recv) <= BufferSize) {

            // PCM buffer not yet full, we stack the samples
            memcpy (buffer + bufferlen, samples, bytes_recv);
            bufferlen += bytes_recv;
          }
          else {

            // PCM buffer full, it is transmitted to the player
            if (! player.write (buffer, bufferlen)) {

              error = player.errorString();
              return -1;
            }

            bufferlen = 0;
            memcpy (buffer, samples, bytes_recv);
            bufferlen += bytes_recv;
          }
        }
      }
      while (PICO_STEP_BUSY == getstatus);

      if (bufferlen) {

        // flush the remaining samples to the player
        if (! player.write (buffer, bufferlen)) {

          error = player.errorString();
          return -1;
        }
        bufferlen = 0;
      }
    }

    return bufferlen;
  }

  // ---------------------------------------------------------------------------
  std::string Tts::Engine::Private::formatText (const std::string & text,
      int volume, int speed, int pitch) {
    std::ostringstream out;
    bool setVolume  = ( (volume != 100)  && (volume >= 0));
    bool setSpeed   = ( (speed != 100)  && (speed  >= 0));
    bool setPitch   = ( (pitch != 100)  && (pitch  >= 0));

    // <volume level="..."> ... </volume>, between 0 and 500
    if (setVolume) {
      if (volume > 500) {
        volume = 500;
      }
      out << "<volume level=\"" << volume << "\">";
    }
    // <speed level="..."> ... </speed>  between 20 and 500
    if (setSpeed) {
      if (speed < 20) {
        speed = 20;
      }
      else if (speed > 500) {
        speed = 500;
      }
      out << "<speed level=\"" << speed << "\">";
    }
    // <pitch level="..."> ... </pitch>, between 50 and 200
    if (setPitch) {
      if (pitch < 50) {
        pitch = 50;
      }
      else if (pitch > 200) {
        pitch = 200;
      }
      out << "<pitch level=\"" << pitch << "\">";
    }
    out << text;
    // <pitch level="..."> ... </pitch>, between 50 and 200
    if (setPitch) {
      out << "</pitch>";
    }
    // <speed level="..."> ... </speed>, between 20 and 500
    if (setSpeed) {
      out << "</speed>";
    }
    // <volume level="..."> ... </volume>, between 0 and 500
    if (setVolume) {
      out << "</volume>";
    }
    out.flush();
    return out.str();
  }

  // ---------------------------------------------------------------------------
  //
  //                        Tts::Engine Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Tts::Engine::Engine (Tts::Engine::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Tts::Engine::Engine () :
    d_ptr (new Private (this)) {}

  // ---------------------------------------------------------------------------
  Tts::Engine::~Engine() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool Tts::Engine::open (const std::string & voiceIso, const std::string & dev) {

    if (!isOpen()) {
      PIMP_D (Tts::Engine);

      if (Voice::exists (voiceIso)) {

        d->voice = Voice::voice (voiceIso);
        if (d->player.open (dev)) {

          return d->open();
        }
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void Tts::Engine::close() {

    if (isOpen()) {
      PIMP_D (Tts::Engine);

      d->close();
      d->player.close();
    }
  }

  // ---------------------------------------------------------------------------
  bool Tts::Engine::write (const std::string & text, int volume, int speed, int pitch) {
    PIMP_D (Tts::Engine);

    d->text2speech = d->formatText (text, volume, speed, pitch);
    return true;
  }

  // ---------------------------------------------------------------------------
  bool Tts::Engine::append (const std::string & text, int volume, int speed, int pitch) {
    PIMP_D (Tts::Engine);

    d->text2speech += d->formatText (text, volume, speed, pitch);
    return true;
  }

  // ---------------------------------------------------------------------------
  bool Tts::Engine::play() {
    PIMP_D (Tts::Engine);

    return d->process() == 0;
  }

  // ---------------------------------------------------------------------------
  const std::string & Tts::Engine::errorString() const {
    PIMP_D (const Tts::Engine);

    return d->error;
  }

  // ---------------------------------------------------------------------------
  const std::string & Tts::Engine::device() const {
    PIMP_D (const Tts::Engine);

    return d->player.device();
  }

  // ---------------------------------------------------------------------------
  bool Tts::Engine::isOpen() const {
    PIMP_D (const Tts::Engine);

    return d->player.isOpen() && (d->engine != 0);
  }

  // ---------------------------------------------------------------------------
  const Tts::Voice & Tts::Engine::voice() const {
    PIMP_D (const Tts::Engine);

    return *d->voice;
  }

  // ---------------------------------------------------------------------------
  std::string Tts::Engine::formatText (const std::string & text, int volume, int speed, int pitch) {

    return Private::formatText (text, volume, speed, pitch);
  }

}
/* ========================================================================== */
