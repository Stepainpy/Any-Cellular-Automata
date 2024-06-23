# Any Cellular Automata (ACA)

This program allows you to create cellular automata based on a given world file.

## Content

- [Quick start](#quick-start)
  - [Build](#build)
  - [Launch](#launch)
- [World file syntax](#world-file-syntax)
  - [As work may statement](#as-work-may-statements)
- [Json gui setting](#json-gui-setting)
- [Modifying 'Game of life' rule](#modifying-game-of-life-rule)
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
```
./aca worlds/gol.world worlds/gui/gol.json
```
For toggle viewing iteration count need press key `F3` (only gui version)

## World file syntax

Words, numbers and symbols separated by spacing symbols (space, tab, new line and etc.).  
Single line comment start by `;`. See the other syntax in the [EBNF](syntax/world.ebnf) and [example file](syntax/example.txt)

### As work may statements

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

Exapmles: [B3/S23](worlds/gol.world), [B35678/S5678](worlds/diamoeba.world)  
For gui use [gol.json](worlds/gui/gol.json)

## TODO

- [x] Add pattern rule
- [x] Add comments in world file
- [ ] Add define statement for symbols