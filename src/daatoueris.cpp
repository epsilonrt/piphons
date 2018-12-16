/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the libdaadtmf Library.
 *
 * The libdaadtmf Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libdaadtmf Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libdaadtmf Library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <piduino/clock.h>
#include <daatoueris.h>
#include "daadtmf_p.h"

// -----------------------------------------------------------------------------
//
//                          DaaToueris::Private Class
//
// -----------------------------------------------------------------------------
class DaaToueris::Private : public Daa::Private {
  public:
    Private (DaaToueris * q, int ringPin, int offhookLed, int offhookSetPin,  int offhookResetPin);
    Pin & offhookSetPin;
    Pin & offhookResetPin;
    virtual void offhook (bool value);
    virtual bool open ();
};

// -----------------------------------------------------------------------------
DaaToueris::Private::Private (DaaToueris * q, int rp, int ohl, int ohsp, int ohrp) :
  Daa::Private (q, rp, ohl, false, true),
  offhookSetPin (gpio.pin (ohsp)), offhookResetPin (gpio.pin (ohrp)) {}

// -----------------------------------------------------------------------------
bool DaaToueris::Private::open() {

  if (gpio.open()) {

    offhookSetPin.setMode (Pin::ModeOutput);
    offhookResetPin.setMode (Pin::ModeOutput);
    offhookSetPin.write (false);
    offhookResetPin.write (false);
    return Daa::Private::open();
  }
  return isOpen;
}

// -----------------------------------------------------------------------------
void DaaToueris::Private::offhook (bool value) {
  Pin & pin = value ? offhookSetPin : offhookResetPin;

  pin.write (true);
  clk.delay (5);
  pin.write (false);
  Daa::Private::offhook (value);
}

// -----------------------------------------------------------------------------
//
//                             DaaToueris Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
DaaToueris::DaaToueris (DaaToueris::Private &dd) : Daa (dd) {}

// -----------------------------------------------------------------------------
DaaToueris::DaaToueris (int rp, int ohl, int ohsp, int ohrp) :
  DaaToueris (*new Private (this, rp, ohl, ohsp, ohrp)) {}

/* ========================================================================== */
