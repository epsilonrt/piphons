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
#ifndef PIPHONS_TTS_ENGINE_PRIVATE_H
#define PIPHONS_TTS_ENGINE_PRIVATE_H

#include <picoapi.h>
#include <picoapid.h>
#include <picoos.h>
#include <piphons/tts.h>

namespace Piphons {

  class Tts::Engine::Private {
    public:
      Private (Engine * q);
      ~Private();
      bool open();
      void close();
      int process();
      static std::string formatText (const std::string & text, int volume, int speed, int pitch);

      static const int MemorySize = 2500000;
      static const int BufferSize = 256;
      static const pico_Char voiceName[];

      Engine * const q_ptr;
      const Voice * voice;

      pico_System system;
      pico_Engine engine;
      void * memory;
      pico_Resource taResource;
      pico_Resource sgResource;
      pico_Char * taResourceName;
      pico_Char * sgResourceName;
      
      std::string text2speech;
      std::string error;
      Player player;

      PIMP_DECLARE_PUBLIC (Engine)
  };
}
/* ========================================================================== */
#endif /* PIPHONS_TTS_ENGINE_PRIVATE_H defined */
