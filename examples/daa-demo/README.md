# [libpiphons](https://github.com/epsilonrt/piphons) DAA example

This example shows how to manage the phone line.  
The program waits for a call, counts the number of rings and picks up 
the phone after 3 rings.  
The program hangs up the phone if the local user presses Enter or the 
other party hangs up the phone.  

To build this program, you must type the command:

    $ g++ -o daa-demo main.cpp $(pkg-config --cflags --libs libpiphons)
    
You can then execute the program :

    $ sudo ./daa-demo

`sudo` is necessary for an access to the memory mapping of the GPIO. 
You can enable the **setuid** bit to avoid `sudo` in the future :

    $ sudo chmod u+s daa-demo
    $ ./daa-demo

With [Codelite](https://codelite.org/) it's easier and funny, right ? 

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/piphons/master/extras/images/codelite.png)

The `libpiphons-dev` package installs a project template in Codelite...
