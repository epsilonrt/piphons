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
#include "tts_voice_p.h"

namespace Piphons {

  // ---------------------------------------------------------------------------
  // static vars
  const char * TtsVoicePaths[] = {
    "/usr/local/share/pico/lang/",
    "/usr/share/pico/lang/",
    0
  };
  const char * TtsVoiceIsoCodes[] = {
    "en-US",
    "en-GB",
    "de-DE",
    "es-ES",
    "fr-FR",
    "it-IT",
    0
  };
  const char * TtsVoiceTaLingware[] = {
    "en-US_ta.bin",
    "en-GB_ta.bin",
    "de-DE_ta.bin",
    "es-ES_ta.bin",
    "fr-FR_ta.bin",
    "it-IT_ta.bin"
  };
  const char * TtsVoiceSgLingware[] = {
    "en-US_lh0_sg.bin",
    "en-GB_kh0_sg.bin",
    "de-DE_gl0_sg.bin",
    "es-ES_zl0_sg.bin",
    "fr-FR_nk0_sg.bin",
    "it-IT_cm0_sg.bin"
  };

  struct LanguageItem {
    const char * name;
    Tts::Voice::Language value;
  };
  LanguageItem TtsVoiceLanguages[] = {
    {"en",  Tts::Voice::en},
    {"de",  Tts::Voice::de},
    {"es",  Tts::Voice::es},
    {"fr",  Tts::Voice::fr},
    {"it",  Tts::Voice::it},
    { 0,    Tts::Voice::ul}
  };

  struct CountryItem {
    const char * name;
    Tts::Voice::Country value;
  };
  CountryItem TtsVoiceCountries[] = {
    {"US",  Tts::Voice::US},
    {"GB",  Tts::Voice::GB},
    {"DE",  Tts::Voice::DE},
    {"ES",  Tts::Voice::ES},
    {"FR",  Tts::Voice::FR},
    {"IT",  Tts::Voice::IT},
    { 0,    Tts::Voice::UC}
  };

  // ---------------------------------------------------------------------------
  //
  //                          Tts::Voice::Private Class
  //
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // static
  bool Tts::Voice::Private::initLock = false;
  std::map<std::string, Tts::Voice> Tts::Voice::Private::voices;
  std::map<std::string, Tts::Voice::Language> Tts::Voice::Private::languages;
  std::map<std::string, Tts::Voice::Country> Tts::Voice::Private::countries;
  std::string Tts::Voice::Private::voicePath;

  // ---------------------------------------------------------------------------
  Tts::Voice::Private::Private (Voice * q,
                                const char * isoCode,
                                const char * taFilename,
                                const char * sgFilename) :
    q_ptr (q),
    iso (isoCode),
    taFile (taFilename),
    sgFile (sgFilename) {

    staticInit();
  }

  // ---------------------------------------------------------------------------
  Tts::Voice::Private::Private (Voice * q) :
    q_ptr (q) {

    staticInit();
  }

  // ---------------------------------------------------------------------------
  Tts::Voice::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  // static
  bool Tts::Voice::Private::setVoicePath (const std::string & path, const std::string & fileToCheck) {

    if (Piduino::System::fileExists (path + fileToCheck)) {
      voicePath = path;
    }
    return ! voicePath.empty();
  }

  // ---------------------------------------------------------------------------
  // static
  void Tts::Voice::Private::staticInit () {

    if (voices.empty() && ! initLock) {
      int i = 0;

      initLock = true; // the first call blocks recursion !

      // populate the voices map
      do {
        Voice v (TtsVoiceIsoCodes[i], TtsVoiceTaLingware[i], TtsVoiceSgLingware[i]);

        if (voicePath.empty()) { // search and checks the lingware ressources directory
          const char * homeDir;
          std::string path;

          homeDir = std::getenv ("HOME");
          if (homeDir) {

            path = std::string (homeDir) + "/.pico/lang/";
            setVoicePath (path, v.taFileName());
          }

          if (voicePath.empty()) {
            int j = 0;

            while (TtsVoicePaths[j]) {
              path.assign (TtsVoicePaths[j]);
              if (setVoicePath (path, v.taFileName())) {
                break;
              }
              j++;
            }

          }
          if (voicePath.empty()) {

            throw std::runtime_error ("Failed to find Pico Langware directory !");
          }
        }

        if (! Piduino::System::fileExists (v.taFilePath())) {

          throw std::runtime_error ("Failed to find Pico Langware ressource file " + v.taFilePath());
        }

        if (! Piduino::System::fileExists (v.sgFilePath())) {

          throw std::runtime_error ("Failed to find Pico Langware ressource file " + v.sgFilePath());
        }

        voices[v.isoCode()] = v;
        i++;
      }
      while (TtsVoiceIsoCodes[i]);

      // populate the languages map
      i = 0;
      do {
        languages[TtsVoiceLanguages[i].name] = TtsVoiceLanguages[i].value;
        i++;
      }
      while (TtsVoiceLanguages[i].name);

      // populate the countries map
      i = 0;
      do {
        countries[TtsVoiceCountries[i].name] = TtsVoiceCountries[i].value;
        i++;
      }
      while (TtsVoiceCountries[i].name);

      initLock = false;
    }
  }

  // ---------------------------------------------------------------------------
  // static
  bool Tts::Voice::Private::exists (const std::string & iso) {

    staticInit();
    return (voices.count (iso) == 1);
  }

  // ---------------------------------------------------------------------------
  // static
  const Tts::Voice * Tts::Voice::Private::voice (const std::string & iso) {

    staticInit();
    if (exists (iso)) {
      return & voices[iso];
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  //
  //                             Tts::Voice Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Tts::Voice::Voice (Tts::Voice::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Tts::Voice::Voice () :
    d_ptr (new Private (this)) {}

  // ---------------------------------------------------------------------------
  Tts::Voice::Voice (const char * isoCode, const char * taFilename, const char * sgFilename) :
    d_ptr (new Private (this, isoCode, taFilename, sgFilename)) {}

  // ---------------------------------------------------------------------------
  Tts::Voice::Voice (const Tts::Voice & other) :
    d_ptr (new Private (*other.d_ptr)) {}

  // ---------------------------------------------------------------------------
  void Tts::Voice::swap (Voice &other) {

    d_ptr.swap (other.d_ptr);
  }

  // ---------------------------------------------------------------------------
  Tts::Voice& Tts::Voice::operator= (const Tts::Voice &other) {

    Voice (other).swap (*this);
    return *this;
  }

  // ---------------------------------------------------------------------------
  Tts::Voice::~Voice() = default;

  // ---------------------------------------------------------------------------
  std::string Tts::Voice::language() const {
    PIMP_D (const Tts::Voice);
    std::string name;
    std::string::size_type n = d->iso.find ('-');

    if (n != std::string::npos) {

      name = d->iso.substr (0, n);
    }
    return name;
  }

  // ---------------------------------------------------------------------------
  Tts::Voice::Language Tts::Voice::languageId() const {
    PIMP_D (const Tts::Voice);
    std::string name = language();

    if (d->languages.count (name) == 1) {
      return d->languages[name];
    }
    return ul;
  }

  // ---------------------------------------------------------------------------
  std::string Tts::Voice::country() const {
    PIMP_D (const Tts::Voice);
    std::string name;
    std::string::size_type n = d->iso.find ('-');

    if (n != std::string::npos) {

      name = d->iso.substr (n + 1);
    }
    return name;
  }

  // ---------------------------------------------------------------------------
  Tts::Voice::Country Tts::Voice::countryId() const {
    PIMP_D (const Tts::Voice);
    std::string name = country();

    if (d->countries.count (name) == 1) {
      return d->countries[name];
    }
    return UC;
  }

  // ---------------------------------------------------------------------------
  std::string Tts::Voice::isoCode() const {
    PIMP_D (const Tts::Voice);

    return d->iso;
  }

  // ---------------------------------------------------------------------------
  void Tts::Voice::setIsoCode (const std::string value) {
    PIMP_D (Tts::Voice);

    d->iso = value;
  }

  // ---------------------------------------------------------------------------
  std::string Tts::Voice::taFilePath() const {

    return Private::voicePath + taFileName();
  }

  // ---------------------------------------------------------------------------
  std::string Tts::Voice::taFileName() const {
    PIMP_D (const Tts::Voice);

    return  d->taFile;
  }

  // ---------------------------------------------------------------------------
  void Tts::Voice::setTaFilename (const std::string value) {
    PIMP_D (Tts::Voice);

    d->taFile = value;
  }

  // ---------------------------------------------------------------------------
  std::string Tts::Voice::sgFilePath() const {

    return Private::voicePath + sgFileName();
  }

  // ---------------------------------------------------------------------------
  std::string Tts::Voice::sgFileName() const {
    PIMP_D (const Tts::Voice);

    return d->sgFile;
  }

  // ---------------------------------------------------------------------------
  void Tts::Voice::setSgFilename (const std::string value) {
    PIMP_D (Tts::Voice);

    d->sgFile = value;
  }

  // ---------------------------------------------------------------------------
  // static
  bool Tts::Voice::exists (const std::string & iso) {

    return Private::exists (iso);
  }

  // ---------------------------------------------------------------------------
  // static
  const Tts::Voice * Tts::Voice::voice (const std::string & iso) {

    return Private::voice (iso);
  }

  // ---------------------------------------------------------------------------
  // static
  bool Tts::Voice::exists (const char * iso) {

    return exists (std::string (iso));
  }

  // ---------------------------------------------------------------------------
  // static
  const Tts::Voice * Tts::Voice::voice (const char * iso) {

    return voice (std::string (iso));
  }

  // ---------------------------------------------------------------------------
  // static
  const std::map<std::string, Tts::Voice> & Tts::Voice::voices() {
    
    Private::staticInit();
    return Private::voices;
  }
}
/* ========================================================================== */
