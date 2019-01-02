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

#ifndef PIPHONS_TTS_H
#define PIPHONS_TTS_H

#include <string>
#include <map>
#include <piphons/global.h>

namespace Piphons {

  /**
   * @class Tts
   * @brief Text to Speech Ic compatible with the MT8870 (HT9170 ...)
   */
  class Tts {
    public:
      class Voice;
      Tts ();
      ~Tts();

      bool open (const std::string & voiceIso = "en-US", const std::string & device = "default");
      bool isOpen() const;
      void close();

      bool say (const std::string & text, int volume = -1, int speed = -1, int pitch = -1);
      const Voice & voice() const;
      const std::string & errorString() const;
      const std::string & device() const;

      class Voice {
        public:
          enum Language {
            en = 0,
            de,
            es,
            it,
            fr,
            ul = -1 // Unknown language
          };
          enum Country {
            US = 0,
            GB,
            DE,
            ES,
            FR,
            IT,
            UC = -1 // Unknown country
          };

          Voice ();
          Voice (const char * isoCode, const char * taFilename, const char * sgFilename);
          Voice (const Voice & other);
          void swap (Voice & other);
          Voice& operator= (const Voice &other);
          ~Voice();

          Language languageId() const;
          std::string language() const;

          Country countryId() const;
          std::string country() const;

          std::string isoCode() const;
          void setIsoCode (const std::string value);

          std::string taFileName() const;
          std::string taFilePath() const;
          void setTaFilename (const std::string value);

          std::string sgFileName() const;
          std::string sgFilePath() const;
          void setSgFilename (const std::string value);

          static bool exists (const std::string & iso);
          static bool exists (const char * iso);

          static const Voice * voice (const std::string & iso);
          static const Voice * voice (const char * iso);
          static const std::map<std::string, Voice> & voices();

        protected:
          class Private;
          Voice (Private &dd);
          std::unique_ptr<Private> d_ptr;

        private:
          PIMP_DECLARE_PRIVATE (Voice)
      };

    protected:
      class Private;
      Tts (Private &dd);
      std::unique_ptr<Private> d_ptr;

      class Player {
        public:

          Player();
          ~Player();
          bool open (const std::string & device);
          bool write (const void * buffer, unsigned long size);
          bool isOpen() const;
          void close();
          const std::string & errorString() const;
          const std::string & device() const;

        protected:
          class Private;
          Player (Private &dd);
          std::unique_ptr<Private> d_ptr;

        private:
          PIMP_DECLARE_PRIVATE (Player)
      };

      class Engine {
        public:

          Engine ();
          ~Engine();

          bool open (const std::string & voiceIso, const std::string & device);
          bool isOpen() const;
          void close();
          const std::string & errorString() const;
          const std::string & device() const;

          bool write (const std::string & text, int volume, int speed, int pitch);
          bool play();

          const Voice & voice() const;

        protected:
          class Private;
          Engine (Private &dd);
          std::unique_ptr<Private> d_ptr;

        private:
          PIMP_DECLARE_PRIVATE (Engine)
      };

    private:
      PIMP_DECLARE_PRIVATE (Tts)
  };

}
/* ========================================================================== */
#endif /* PIPHONS_TTS_H defined */
