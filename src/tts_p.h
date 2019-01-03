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
#ifndef PIPHONS_TTS_PRIVATE_H
#define PIPHONS_TTS_PRIVATE_H

#include <shared_mutex>
#include <future>
#include <thread>
#include <piduino/threadsafebuffer.h>
#include <piphons/tts.h>

namespace Piphons {

  class Tts::Private {
    public:
      Private (Tts * q);
      ~Private();
      static void * parser (const std::string & voiceIso,
                            const std::string & device,
                            std::future<void> run,
                            Private * d);

      Tts * const q_ptr;
      Engine engine;

      std::string text;
      std::promise<void> stopParsing;
      std::thread parseThread;
      Piduino::ThreadSafeBuffer<std::string> fifo;

      PIMP_DECLARE_PUBLIC (Tts)
  };
}
/* ========================================================================== */
#endif /* PIPHONS_TTS_PRIVATE_H defined */
