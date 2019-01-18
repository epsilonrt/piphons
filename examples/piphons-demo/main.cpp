// libpiphons DTMF/TTS example
//
// This example shows how to manage a voice server.
// The program waits for a call, counts the number of rings and picks up
// the phone after 3 rings.
// After a welcome message, the program waits for the other party to press
// keys and diffuse a different message for each one.
// If the other party presses 2 # keys successively, the server says a
// goodbye message and hangs up the phone.
//
// This example code is in the public domain.
#include <iostream>
#include <string>
#include <atomic>
#include <piphons.h>
#include <Arduino.h> // Piduino Arduino API

using namespace std;
using namespace Piphons;

const int ringPin = 6;  // Header Pin 22: GPIO6 for RPi, GPIOA1 for NanoPi
const int offhookPin = 5; // Header Pin 18: GPIO7 for RPi, GPIOG9 for NanoPi
const int tonePin = 11; // Header Pin 26: GPIO24 for RPi, GPIOA17 for NanoPi
const std::array<int, 5> dtmfPins = {0, 1, 2, 3, 4}; // D0, D1, D2, D3, DV

atomic<bool> loop (true); // Atomic Boolean variable, needed because the handlers run in different threads...
Tts tts; // object for speech synthesis, to talk to each other.

// -----------------------------------------------------------------------------
void ringIsr (Daa * daa) { // this function is executed at each ring

  // displays the number of rings since the last call
  cout << daa->ringingSinceHangup() << flush;
}

// -----------------------------------------------------------------------------
void offhookIsr (Daa * daa) { // this function is executed after hookoff

  cout << endl
       << "Off-hook !" << endl
       << "Waiting for DTMF key... " << endl
       << "Send ## to hang up" << endl;
  delay (2000); // delay for the establishment of the telephone line.
  tts.say ("Bonjour, Bienvenue à bord du Nano Pi.", 100, 80);
  tts.say ("Je m'appelle R2D2.", 80, 80);
  tts.say ("Veuillez appuyer sur des touches.", 80, 80);
  tts.say ("Deux dièses permettent de raccrocher.", 80, 80);
}

// -----------------------------------------------------------------------------
void hangupIsr (Daa * daa) { // this function is executed after hangup

  cout << endl << "Remote user hangup !" << endl;
  loop = false; // drop the flag atomically to tell the main thread to stop
}

// -----------------------------------------------------------------------------
void keyIsr (Dtmf * dtmf) {
  static char prev;
  char c;
  string text;

  c = dtmf->read(); // get the key
  cout << c << flush; // print the key to stdout

  delay (100); // avoid the message being spoken during the beep
  // says a message depending on which key is pressed
  switch (c) {
    case '1':
      text = "Arrêt.";
      break;
    case '2':
      text = "Marche.";
      break;
    case '3':
      text = "Fait le ! ou ne le fait pas. Il n'y a pas d'essai.";
      break;
    case '#':
      text = "Dièse.";
      break;
    default:
      text = c; // say the number corresponding to the key
  }
  tts.say (text);

  if ( (c == '#') && (prev == '#')) {
    // 2 # sent consecutively, this is the signal to stop

    tts.say ("Au revoir !"); // goodbye message
    loop = false; // drop the flag atomically to tell the main thread to stop
    cout << endl << "I hung up the phone." << endl;
  }

  prev = c; // memorizes the previous character
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  Daa daa (ringPin, offhookPin, tonePin);
  Dtmf dtmf (dtmfPins);

  daa.setRingingHandler (ringIsr);
  daa.setOffhookHandler (offhookIsr);
  daa.setHangupToneHandler (hangupIsr);
  daa.setHookFlash (true);
  daa.setRingingBeforeOffhook (3);
  daa.setHangupToneDetect (true, 6, 450, 650);
  daa.open();

  dtmf.setKeyHandler (keyIsr);
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
  cout << endl << "Have a nice day !" << endl;

  return 0;
}
/* ========================================================================== */
