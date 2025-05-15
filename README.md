### \* In development \*
Estimated release date: 3045

### What is it?
The application for obtaining data from the Steam market (the number of skins on the trading platform, the current prices and the number of offers), sending them to the PGSGL server, as well as telegrams in which you can track the price of inventory and separately items. At the moment, the application is aimed at the Rust game, but in the future it will be possible to change the game. The program is located at an early stage of development, the code is very dirty and not optimized, in further functionality will expand, and optimization improves. At the moment, this is my main project

### Build:  
1. sudo apt update
2. sudo apt install cmake
3. sudo apt install g++
4. sudo apt install qt6-webengine-dev
5. sudo apt install libpq-dev

Also, for this to work, you need to download and run: [Tor](https://community.torproject.org/onion-services/setup/install/) and [TorNet](https://github.com/ByteBreach/tornet)  
(This is necessary in order to change IP addresses every second.)  

If you use it on VDS, you need to install Xvfb

Working version of the bot: @RustSteamHelperBot
