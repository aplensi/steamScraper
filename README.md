### \* In development \*
Estimated release date: 3045

### What is it?
An application for receiving data from the Steam marketplace and sending it to the pgsgl server.  
Ultimately, this application should become a telegram bot, in which it will be possible to monitor changes in the price of inventory and, in general, some analysis of the marketplace (for now, the application is aimed at the Rust game, but in the future it will be possible to change the game).

### Build:  
1. sudo apt update
2. sudo apt install cmake
3. sudo apt install g++
4. sudo apt install qt6-webengine-dev
5. sudo apt install libpq-dev

Also, for this to work, you need to download and run: [Tor](https://community.torproject.org/onion-services/setup/install/) and [TorNet](https://github.com/ByteBreach/tornet)  
(This is necessary in order to change IP addresses every second.)