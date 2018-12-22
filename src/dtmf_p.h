/* Copyright © 2018 Pascal JEAN, All rights reserved.
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
#ifndef PIPHONS_DTMF_PRIVATE_H
#define PIPHONS_DTMF_PRIVATE_H

#include <shared_mutex>
#include <queue>
#include <string>
#include <piduino/gpio.h>
#include <piduino/gpiopin.h>
#include <piphons/dtmf.h>

using namespace Piduino;

namespace Piphons {

  class Dtmf::Private {
    public:
      Private (Dtmf * q, int d0Pin, int d1Pin, int d2Pin, int d3Pin, int dvPin);
      Private (Dtmf * q, const std::array<int,5> & dPin);
      Private (const Private & other);
      virtual ~Private();

      Dtmf * const q_ptr;
      std::array<Pin *, 4> dPin;
      Pin & dvPin;
      DtmfHandler userKeyHandler;
      bool isOpen;
      std::queue<char> fifo;
      // must remain the last ...
      mutable std::shared_timed_mutex mut;

      virtual bool open ();
      virtual void close ();

      static void keyIsr (void * dtmf);
      static const std::string keys;

      PIMP_DECLARE_PUBLIC (Dtmf)
  };

}
/* ========================================================================== */
#endif /* PIPHONS_DTMF_PRIVATE_H defined */
