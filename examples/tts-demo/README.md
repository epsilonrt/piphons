# [libpiphons](https://github.com/epsilonrt/piphons) Text to speech (Tts) example

This example shows how to use the text to speech Tts class.  
This program says a message on the jack output.

To build this program, you must type the command:

    $ g++ -o tts-demo main.cpp $(pkg-config --cflags --libs libpiphons)
    
You can then execute the program :

    $ ./tts-demo "Bonjour, comment allez-vous ?"

With [Codelite](https://codelite.org/) it's easier and funny, right ? 

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/piphons/master/extras/images/codelite.png)

The `libpiphons-dev` package installs a project template in Codelite...
