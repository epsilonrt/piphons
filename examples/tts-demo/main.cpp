// libpiphons TTS example
//
// This example shows how to use the text to speech Tts class
// this program says a message on the jack output.
//
// This example code is in the public domain.
#include <iostream>
#include <string>
#include <piphons.h>

using namespace std;
using namespace Piphons;

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  Tts tts; // object for speech synthesis, to talk to each other.

  std::string text ("Fait le ! ou ne le fait pas. Il n'y a pas d'essai.");

  if (argc > 1) {
    text = argv[1];
  }

  cout  << "Tts say <" <<  text << "> on " << tts.device() << " device." << endl;
  tts.open ("fr-FR");
  tts.say (text, 100, 80);
  tts.close();
  cout  << "Have a nice day !" << endl;

  return 0;
}
/* ========================================================================== */
