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
#include <piduino/clock.h>
#include <piphons/toueris/daa.h>
#include "daa_p.h"

namespace Toueris {

  // ---------------------------------------------------------------------------
  //
  //                          Toueris::Daa::Private Class
  //
  // ---------------------------------------------------------------------------
  class Daa::Private : public Piphons::Daa::Private {
    public:
      Private (Daa * q, int ringPin, int offhookLed, int offhookSetPin,  int offhookResetPin);
      Pin & offhookSetPin;
      Pin & offhookResetPin;
      virtual void offhook (bool value);
      virtual bool open ();
  };

  // ---------------------------------------------------------------------------
  Daa::Private::Private (Daa * q, int rp, int ohl, int ohsp, int ohrp) :
    Piphons::Daa::Private (q, rp, ohl, false, true),
    offhookSetPin (gpio.pin (ohsp)), offhookResetPin (gpio.pin (ohrp)) {}

  // ---------------------------------------------------------------------------
  bool Daa::Private::open() {

    if (gpio.open()) {

      offhookSetPin.setMode (Pin::ModeOutput);
      offhookResetPin.setMode (Pin::ModeOutput);
      offhookSetPin.write (false);
      offhookResetPin.write (false);
      return Piphons::Daa::Private::open();
    }
    return isOpen;
  }

  // ---------------------------------------------------------------------------
  void Daa::Private::offhook (bool value) {
    Pin & pin = value ? offhookSetPin : offhookResetPin;

    pin.write (true);
    clk.delay (5);
    pin.write (false);
    Piphons::Daa::Private::offhook (value);
  }

  // ---------------------------------------------------------------------------
  //
  //                             Toueris::Daa Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Daa::Daa (Daa::Private &dd) : Piphons::Daa (dd) {}

  // ---------------------------------------------------------------------------
  Daa::Daa (int rp, int ohl, int ohsp, int ohrp) :
    Daa (*new Private (this, rp, ohl, ohsp, ohrp)) {}

}
/* ========================================================================== */
