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
#include "dtmf_p.h"
#include <piduino/clock.h>

namespace Piphons {

  // ---------------------------------------------------------------------------
  //
  //                          Dtmf::Private Class
  //
  // ---------------------------------------------------------------------------
  const std::string Dtmf::Private::keys ("D1234567890*#ABC");


  // ---------------------------------------------------------------------------
  Dtmf::Private::Private (Dtmf * q, int d0, int d1, int d2, int d3, int dv) :
    q_ptr (q),
    dvPin (gpio.pin (dv)) ,
    userKeyHandler (0),
    isOpen (false) {

    dPin[0] = &gpio.pin (d0);
    dPin[1] = &gpio.pin (d1);
    dPin[2] = &gpio.pin (d2);
    dPin[3] = &gpio.pin (d3);
  }

  // ---------------------------------------------------------------------------
  Dtmf::Private::Private (Dtmf * q, const std::array<int,5> & pin) :
    q_ptr (q),
    dvPin (gpio.pin (pin.at (4))) ,
    userKeyHandler (0),
    isOpen (false) {

    for (int i = 0; i < dPin.size(); i++)   {
      dPin[i] = &gpio.pin (pin.at (i));
    }
  }

  // ---------------------------------------------------------------------------
  Dtmf::Private::Private (const Dtmf::Private & other) :
    q_ptr (other.q_ptr),
    dPin (other.dPin),
    dvPin (other.dvPin) {

    // requires exclusive ownership to write to *this
    std::unique_lock<std::shared_timed_mutex> lhs (mut, std::defer_lock);
    // requires shared ownership to read from other
    std::shared_lock<std::shared_timed_mutex> rhs (other.mut, std::defer_lock);
    std::lock (lhs, rhs);

    // assign data
    userKeyHandler = other.userKeyHandler;
    isOpen = other.isOpen;
    fifo = other.fifo;
  }

  // ---------------------------------------------------------------------------
  Dtmf::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  bool Dtmf::Private::open() {

    if (gpio.open()) {

      dvPin.setMode (Pin::ModeInput);
      dvPin.attachInterrupt (Private::keyIsr, Pin::EdgeRising, this);
      for (auto & p : dPin)   {
        p->setMode (Pin::ModeInput);
      }
      isOpen = true;
    }
    return isOpen;
  }

  // ---------------------------------------------------------------------------
  void Dtmf::Private::close() {

    dvPin.detachInterrupt();
    isOpen = false;
  }

  // ---------------------------------------------------------------------------
  void Dtmf::Private::keyIsr (void * data) {
    Private * d;
    DtmfHandler userKeyHandler;
    uint8_t k = 0, m = 1;

    d = reinterpret_cast<Dtmf::Private *> (data);
    d->dvPin.read();  // phony reading, to erase irq !

    for (auto & p : d->dPin)   {
      if (p->read ()) {
        k |= m;
      }
      m <<= 1;
    }

    if (k < keys.size()) {

      char c = keys[k];
      {
        // requires exclusive ownership to write to ringingSinceHangup
        std::lock_guard<std::shared_timed_mutex> lk (d->mut);
        d->fifo.push (c);
      }
      {
        // requires shared ownership to read from other this data
        std::shared_lock<std::shared_timed_mutex> lk (d->mut);
        userKeyHandler = d->userKeyHandler;
      }
      if (userKeyHandler) {

        userKeyHandler (d->q_ptr);
      }
    }
  }

  // ---------------------------------------------------------------------------
  //
  //                             Dtmf Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Dtmf::Dtmf (Dtmf::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Dtmf::Dtmf (int d0, int d1, int d2, int d3, int dv) :
    d_ptr (new Private (this, d0, d1, d2, d3, dv)) {}

  // ---------------------------------------------------------------------------
  Dtmf::Dtmf (const std::array<int,5> & dPin) :
    d_ptr (new Private (this, dPin)) {}

  // ---------------------------------------------------------------------------
  Dtmf::Dtmf (const Dtmf & other) :
    d_ptr (new Private (*other.d_ptr)) {}

  // ---------------------------------------------------------------------------
  void Dtmf::swap (Dtmf &other) {

    d_ptr.swap (other.d_ptr);
  }

  // ---------------------------------------------------------------------------
  Dtmf& Dtmf::operator= (const Dtmf &other) {

    Dtmf (other).swap (*this);
    return *this;
  }

  // ---------------------------------------------------------------------------
  Dtmf::~Dtmf() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool Dtmf::open() {

    if (!isOpen()) {
      PIMP_D (Dtmf);

      return d->open();
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  void Dtmf::close() {

    if (isOpen()) {
      PIMP_D (Dtmf);

      d->close();
    }
  }

  // ---------------------------------------------------------------------------
  bool Dtmf::isOpen() const {
    PIMP_D (const Dtmf);

    return d->isOpen;
  }

  // ---------------------------------------------------------------------------
  void Dtmf::setKeyHandler (DtmfHandler handler) {
    PIMP_D (Dtmf);

    std::lock_guard<std::shared_timed_mutex> lk (d->mut);
    d->userKeyHandler = handler;
  }

  // ---------------------------------------------------------------------------
  std::size_t Dtmf::available() const {
    PIMP_D (const Dtmf);

    std::shared_lock<std::shared_timed_mutex> lk (d->mut);
    return d->fifo.size();
  }

  // ---------------------------------------------------------------------------
  int Dtmf::read() {
    PIMP_D (Dtmf);

    std::lock_guard<std::shared_timed_mutex> lk (d->mut);
    if (!d->fifo.empty()) {
      unsigned int c = static_cast<unsigned char> (d->fifo.front());
      d->fifo.pop();
      return c;
    }
    return -1;
  }
}

/* ========================================================================== */
