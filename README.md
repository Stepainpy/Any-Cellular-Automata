# Any Cellular Automata (ACA)

This program allows you to create cellular automata based on a given world file.

## Build

``` console
mkdir build
cmake -S . -B build
cmake --build build
```

## Launch

``` console
./aca worlds/gol.world
```
This is app run in console

## World file syntax

Words, numbers and symbols separated by spacing symbols (space, tab, new line and etc.).  
Single line comment start by `;`. See the other syntax in the [EBNF](syntax/world.ebnf) and [example file](syntax/example.txt)

## TODO

- [x] Add pattern rule
- [x] Add comments in world file