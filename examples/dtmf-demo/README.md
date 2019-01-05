# [libpiphons](https://github.com/epsilonrt/piphons) DTMF example

This example shows how to manage a DTMF key decoder.  
The program waits for a call, counts the number of rings and picks up 
the phone after 3 rings.  
Then the program waits for the other party to press keys and display them.  
If the other party presses 2 # keys successively, the program hangs up the phone.  

To build this program, you must type the command:

    $ g++ -o dtmf-demo main.cpp $(pkg-config --cflags --libs libpiphons)
    
You can then execute the program :

    $ sudo ./dtmf-demo

`sudo` is necessary for an access to the memory mapping of the GPIO. 
You can enable the **setuid** bit to avoid `sudo` in the future :

    $ sudo chmod u+s dtmf-demo
    $ ./dtmf-demo

With [Codelite](https://codelite.org/) it's easier and funny, right ? 

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/piphons/master/extras/images/codelite.png)

The `libpiphons-dev` package installs a project template in Codelite...
