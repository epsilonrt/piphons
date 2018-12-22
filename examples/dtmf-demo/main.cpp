// libpiphons DTMF example
// This example code is in the public domain.
#include <iostream>
#include <atomic>
#include <piphons/daa.h>
#include <piphons/dtmf.h>
#include <Arduino.h> // Piduino Arduino API

using namespace std;
using namespace Piphons;

const int ringPin = 6;  // Header Pin 22: GPIO6 for RPi, GPIOA1 for NanoPi
const int offhookPin = 22; // Header Pin 31: GPIO22 for RPi, GPIOA21 for NanoPi
const int offhookSetPin = 30;
const int offhookResetPin = 21;
const std::array<int, 5> dtmfPin = {7, 0, 2, 3, 31}; // D0,D1,D2,D3,DV

std::atomic<bool> loop (true);

// -----------------------------------------------------------------------------
void ringIsr (Daa * daa) {

  cout << daa->ringingSinceHangup() << flush;
}

// -----------------------------------------------------------------------------
void offhookIsr (Daa * daa) {

  cout << endl
       << "Off-hook !" << endl
       << "Waiting for DTMF key... " << endl
       << "Send ## to hang up" << endl;
}

// -----------------------------------------------------------------------------
void keyIsr (Dtmf * dtmf) {
  static char prev;
  char c;

  c = dtmf->read();
  cout << c << flush;
  if ( (c == '#') && (prev == '#')) {
    loop = false;
  }
  prev = c;
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  Daa daa (ringPin, offhookPin, false, true);
  Dtmf dtmf (dtmfPin);

  dtmf.setKeyHandler (keyIsr);
  daa.setRingingHandler (ringIsr);
  daa.setOffhookHandler (offhookIsr);
  daa.setHookFlash (true);
  daa.open();
  dtmf.open();

  cout << "Piphons DTMF Example" << endl
       << "Waiting for a call..." << endl
       << "Press CTRL+C to quit !" << endl;
  while (loop.load()) {

    delay (100);
  }

  daa.close();
  dtmf.close();
  cout << endl
       << "I hung up the phone." << endl
       << "Have a nice day !" << endl;

  return 0;
}
/* ========================================================================== */
