# Commands

## Cell

Syntax
```
cell <set-char> <x> <y>
```

Arguments
|   Name   |   Type   | Description                |
| :------: | :------: | :------------------------- |
| set-char | `symbol` | value for setting in world |
|    x     | `number` | X position                 |
|    y     | `number` | Y position                 |

## Linex

Syntax
```
linex <set-char> <x0> <y> <x1>
```

Arguments
|   Name   |   Type   | Description                |
| :------: | :------: | :------------------------- |
| set-char | `symbol` | value for setting in world |
|    x0    | `number` | X begin position           |
|    y     | `number` | Y position                 |
|    x1    | `number` | X end position             |

Example  
Command: `linex '#' 3 0 6`
```
Index: 0 1 2 3 4 5 6 7 8 9
Value: . . . # # # # . . .
```

## Liney

Syntax
```
liney <set-char> <x> <y0> <y1>
```

Arguments
|   Name   |   Type   | Description                |
| :------: | :------: | :------------------------- |
| set-char | `symbol` | value for setting in world |
|    x     | `number` | X position                 |
|    y0    | `number` | Y begin position           |
|    y1    | `number` | Y end position             |

Example  
Command: `liney '#' 0 3 6`
```
Index: 0 1 2 3 4 5 6 7 8 9
Value: . . . # # # # . . .
```

## Rect

Syntax
```
rect <set-char> <x> <y> <width> <height> 
```

Arguments
|   Name   |   Type   | Description                |
| :------: | :------: | :------------------------- |
| set-char | `symbol` | value for setting in world |
|    x     | `number` | left side position         |
|    y     | `number` | up side position           |
|  width   | `number` | rectangle width            |
|  height  | `number` | rectangle height           |

## Random

Syntax
```
random <x> <y> <width> <height> 
```

Arguments
|   Name   |   Type   | Description                |
| :------: | :------: | :------------------------- |
| set-char | `symbol` | value for setting in world |
|    x     | `number` | left side position         |
|    y     | `number` | up side position           |
|  width   | `number` | rectangle width            |
|  height  | `number` | rectangle height           |

Fill area in setted rectangle random symbols from world alphabet

## Pattern

Syntax
```
pattern <x> <y> <width> <height> <pattern-char>...
```

Arguments
|      Name       |         Type         | Description               |
| :-------------: | :------------------: | :------------------------ |
|        x        |       `number`       | left side position        |
|        y        |       `number`       | up side position          |
|      width      |       `number`       | rectangle width           |
|     height      |       `number`       | rectangle height          |
| pattern-char... | `symbol`<br>`phrase` | quantity = width * height |

`pattern-char` maybe as symbol, as keyword `any`. If paste `any`, then symbol in world not changed. Sequence read row by row