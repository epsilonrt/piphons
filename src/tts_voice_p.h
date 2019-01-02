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
#ifndef PIPHONS_TTS_VOICE_PRIVATE_H
#define PIPHONS_TTS_VOICE_PRIVATE_H

#include <map>
#include <piphons/tts.h>

namespace Piphons {

  class Tts::Voice::Private {
    public:
      Private (Voice * q);
      Private (Voice * q, const char * isoCode, const char * taFilename, const char * sgFilename);
      ~Private();

      Tts::Voice * const q_ptr;
      std::string iso;
      std::string taFile;
      std::string sgFile;

      static void staticInit();
      static bool setVoicePath (const std::string & path, const std::string & fileToCheck);
      static bool exists (const std::string & iso);
      static const Voice * voice (const std::string & iso);

      static bool initLock;
      static std::map<std::string, Voice> voices;
      static std::map<std::string, Language> languages;
      static std::map<std::string, Country> countries;
      static std::string voicePath;

      PIMP_DECLARE_PUBLIC (Voice)
  };
}
/* ========================================================================== */
#endif /* PIPHONS_TTS_VOICE_PRIVATE_H defined */
