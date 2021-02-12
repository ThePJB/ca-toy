# CA-Toy
Its sort of a framework for cellular automatas written in C, using SDL.

## Command Line Args
 * --width (default 640)
 * --height (default 480)
 * --scale (default 1)

## Controls
 * ESC to quit
 * m for menu
 * 1-4 to switch CA's
 * r to reset
 * hjkl for menu navigation

 ## Current CA's
 1. Predator Prey
 2. Biham–Middleton–Levine traffic model (https://en.wikipedia.org/wiki/Biham%E2%80%93Middleton%E2%80%93Levine_traffic_model)
 3. Elementary CA's (https://en.wikipedia.org/wiki/Elementary_cellular_automaton) with single on pixel start
 4. Elementary CA's with randomized start

## CA ideas
* Elementary CA's++: resulting state also depends on previous value of the pixel (4 bits), or previous neighbourhood (6 bits), or on a wider neighbourhood (5 bits)
* elementary random walker - just randomly change the rule periodically (and re randomize when it hits a crap one like rule 0)
* Life, duh, it's more that I dont yet support edit, save and load but there's no reason I couldn't I suppose...
* Bushfires 
* https://en.wikipedia.org/wiki/Cyclic_cellular_automaton
* Some evolutionary game theory sims like hawks and doves https://en.wikipedia.org/wiki/Evolutionary_game_theory
* There's another traffic CA called the Nagel–Schreckenberg model I think
* langton's ant
* something that simulates the rise and fall of empires would be awesome...

## Architecture
 * main is for dependencies and CLAP
 * application is sort of the main struct for the application, duh, anything that would normally have been global soup before goes here
 * 'gef' is a made up name for the SDL wrapper
 * CA's are pretty modular, they are in the rules subdirectory, they have to satisfy the rule 'interface' from application.h
 * the rules array in application.c is where they are registered. Should this be a member of application? maybe

## Todos
 * scaling and panning
 * speed (steps per frame)
 * menu for these things
 * clap for res and scale maybe (draw just needs to know scale, xres, yres etc)