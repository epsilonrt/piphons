// libpiphons DTMF/TTS example
// This example code is in the public domain.
#include <iostream>
#include <string>
#include <atomic>
#include <piphons.h>
#include <Arduino.h> // Piduino Arduino API

using namespace std;
using namespace Piphons;

const int ringPin = 6;  // Header Pin 22: GPIO6 for RPi, GPIOA1 for NanoPi
const int offhookPin = 22; // Header Pin 31: GPIO22 for RPi, GPIOA21 for NanoPi
const int offhookSetPin = 30;
const int offhookResetPin = 21;
const std::array<int, 5> dtmfPin = {7, 0, 2, 3, 31}; // D0,D1,D2,D3,DV

std::atomic<bool> loop (true);
Tts tts;

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
  tts.say ("Bonjour, veuillez saisir le code secret.");
}

// -----------------------------------------------------------------------------
void keyIsr (Dtmf * dtmf) {
  static char prev;
  char c;
  string text;

  c = dtmf->read();
  cout << c << flush;
  switch (c) {
    case '*':
      text = "étoile.";
      break;
    case '#':
      text = "dièze.";
      break;
    default:
      text = c;
  }
  tts.say (text);

  if ( (c == '#') && (prev == '#')) {
    loop = false;
  }
  prev = c;
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  // Toueris::Daa daa (ringPin, offhookPin, offhookSetPin, offhookResetPin);
  Daa daa (ringPin, offhookPin, false, true);
  Dtmf dtmf (dtmfPin);

  dtmf.setKeyHandler (keyIsr);
  daa.setRingingHandler (ringIsr);
  daa.setOffhookHandler (offhookIsr);
  daa.setHookFlash (true);
  daa.open();
  dtmf.open();
  tts.open ("fr-FR");

  cout << "Piphons DTMF/TTS Example" << endl
       << "Waiting for a call..." << endl
       << "Press CTRL+C to quit !" << endl;
  while (loop.load()) {

    delay (100);
  }

  tts.close();
  dtmf.close();
  daa.close();
  cout << endl
       << "I hung up the phone." << endl
       << "Have a nice day !" << endl;

  return 0;
}
/* ========================================================================== */
