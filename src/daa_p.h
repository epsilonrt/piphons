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
#ifndef PIPHONS_DAA_PRIVATE_H
#define PIPHONS_DAA_PRIVATE_H

#include <shared_mutex>
#include <cerrno>
#include <piduino/gpio.h>
#include <piduino/gpiopin.h>
#include <piphons/daa.h>

using namespace Piduino;

namespace Piphons {

  class Daa::Private {
    public:
      Private (Daa * q, int ringPin, int offhookPin, int tonePin);
      Private (Daa * q, int ringPin, int offhookPin, bool ringEnabledLevel, bool offhookEnabledLevel);
      Private (const Private & other);
      virtual ~Private();

      Daa * const q_ptr;
      Pin & ringPin;
      Pin & offhookPin;
      Pin * tonePin;
      bool ringEnabledLevel;
      bool offhookEnabledLevel;
      int ringingBeforeOffhook;
      int ringingSinceHangup;
      bool toneHangupEnable;
      unsigned long tonePwMin;
      unsigned long tonePwMax;
      int tonePulseCount;
      unsigned long lastTone;
      int tonePulseBeforeHanghup;
      DaaHandler userRingingHandler;
      DaaHandler userOffhookHandler;
      DaaHandler userHangupToneHandler;
      unsigned long lastRinging;
      bool hookFlash;
      bool isOpen;
      // must remain the last ...
      mutable std::shared_timed_mutex mut;

      virtual bool isOffhook () const;
      virtual void offhook (bool value);
      virtual bool open ();
      virtual void close ();

      static void ringIsr (void * daa);
      static void toneIsr (void * daa);

      PIMP_DECLARE_PUBLIC (Daa)
  };

}
/* ========================================================================== */
#endif /* PIPHONS_DAA_PRIVATE_H defined */
