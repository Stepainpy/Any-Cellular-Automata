# Any Cellular Automata (ACA)

This program allows you to create cellular automata based on a given world file.

## Content

- [Quick start](#quick-start)
  - [Build](#build)
  - [Launch](#launch)
- [World file syntax](#world-file-syntax)
  - [How work may statement](#how-work-may-statements)
- [Json gui setting](#json-gui-setting)
- [Modifying 'Game of life' rule](#modifying-game-of-life-rule)
- [Implemented automata](#implemented-automata)
- [Todo](#todo)

## Quick start

### Build

``` console
mkdir build
cmake -S . -B build
cmake --build build
```

### Launch

As console app
``` console
./aca worlds/gol.world
```

As graphich app (use [Raylib](https://github.com/raysan5/raylib))
``` console
./aca worlds/gol.world worlds/gui/gol.json
```

For toggle viewing iteration count need press key `F3`

## World file syntax

Words, numbers and symbols separated by spacing symbols (space, tab, new line and etc.), Tabulation and line feed it does not matter.  
See the other syntax in the [EBNF](syntax/world.ebnf), [example file](syntax/example.txt) and [documentation](doc/statements.md).

### How work may statements

may statement in if block checking via `and`
```
'a' may 1 2 3
'b' may 4 5 6
'c' may 7 8 9
```
as `'a' and 'b' and 'c'`

numbers in may statement checking via `or`  
`'a' may 1 2 3` as `1 or 2 or 3`

## Json gui setting

Show example with ['Game of life' gui](worlds/gui/gol.json)
``` json
{
    "pixel": {
        "width" : 10,
        "height": 10
    },
    "dict": {
        ".": [ 24,  24,  24],
        "#": [231, 231, 231]
    }
}
```
Where field:
- `pixel` contains self size
- `dict` contains matched char with color (in RGB)

Colors in `dict` must be unique because char matched to color and color matched to char

## Modifying 'Game of life' rule

Rules for original game maybe present as `B3/S23`, where `B` - birth, `S` - survive  

Template:
```
world 80 30 '.' 2
'.' ; dead
'#' ; live
alias end
rules CountRule
    state '.' to '#' if '#' may
        ; numbers in 'B'
    end
    state '#' to '.' if '#' nomay
        ; numbers in 'S'
    end
end
setup
    ; your setup of world
end
```

Exapmles: [B3/S23](worlds/gol.world), [B35678/S5678](worlds/diamoeba.world), [B2/S](worlds/seeds.world), [B3678/S34678](worlds/day_and_night.world)  
For gui use [gol.json](worlds/gui/gol.json)

## Implemented automata

- [Conway's Game of Life](worlds/gol.world), [Wikipedia](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
  - [with tail cells](worlds/gol_with_tail.world)
- [Life-like cellular automaton](https://en.wikipedia.org/wiki/Life-like_cellular_automaton)
  - [Day and Night](worlds/day_and_night.world), [Wikipedia](https://en.wikipedia.org/wiki/Day_and_Night_(cellular_automaton))
  - [Diamoeba](worlds/diamoeba.world)
  - [Seeds](worlds/seeds.world), [Wikipedia](https://en.wikipedia.org/wiki/Seeds_(cellular_automaton))
- [Brian's Brain](worlds/brian_brain.world), [Wikipedia](https://en.wikipedia.org/wiki/Brian%27s_Brain)
- Langton's ant, [Wikipedia](https://en.wikipedia.org/wiki/Langton%27s_ant)
  - [69 x 69](worlds/langton_ant.world)
  - [201 x 201](worlds/langton_ant_big.world)
- [Wireworld](worlds/wireworld.world), [Wikipedia](https://en.wikipedia.org/wiki/Wireworld)
- [Belousov–Zhabotinsky reaction (simple)](worlds/reactionBZ.world), [Wikipedia](https://en.wikipedia.org/wiki/Belousov–Zhabotinsky_reaction)

## TODO

- [x] Add pattern rule
- [x] Add comments in world file
- [x] Add alias statement for symbols