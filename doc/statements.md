# Statements

## File stuct

```
<world statement>
<rules statement>
<setup statement>
```

## World statement

Syntax
```
world <width> <height> <fill-char> <alphabet-size> <alphabet-char>...
```

Arguments
|     Name      |   Type   | Description                                                                                               |
| :-----------: | :------: | :-------------------------------------------------------------------------------------------------------- |
|     width     | `number` | world width                                                                                               |
|    height     | `number` | world height                                                                                              |
|   fill-char   | `symbol` | start fill char                                                                                           |
| alphabet-size | `number` | size alphabet for world                                                                                   |
| alphabet-char | `symbol` | sequence symbols with <br> length equal `alphabet-size` <br> (view [example](../worlds/reactionBZ.world)) |

## Rules statement

Syntax
```
rules <rule-name>
    <state statement>
    ...
end
```

Argument
|   Name    |   Type   | Description                                        |
| :-------: | :------: | :------------------------------------------------- |
| rule-name | `phrase` | used rule type, maybe `CountRule` or `PatternRule` |

## State statement

Syntax  
`CountRule`
```
state <before-char> to <after-char> if
    <may statement>
    ...
end
```
`PatternRule`
```
state <before-char> to <after-char> if <pattern statement> end
```

Arguments
|    Name     |   Type   | Description                     |
| :---------: | :------: | :------------------------------ |
| before-char | `symbol` | the symbol that will be changed |
| after-char  | `symbol` | The symbol he will become       |

## May statement

Syntax
```
<need-char> may <quantity>...
```
or
```
<need-char> nomay <quantity>...
```

Arguments
|   Name    |   Type   | Description                                    |
| :-------: | :------: | :--------------------------------------------- |
| need-char | `symbol` | requiritur symbol                              |
| quantity  | `number` | set of maybe count neighboors, <br> set maybe empty |

if is `nomay`, then set of maybe count neighboors equal
$$
\set{ 0, 1, \cdots, 8 } \setminus quantity
$$
where `\` - set minus ([Wikipedia](https://en.wikipedia.org/wiki/Complement_(set_theory)))

## Pattern statement

Syntax
```
<pattern-char> <pattern-char> <pattern-char> <pattern-char> <pattern-char> <pattern-char> <pattern-char> <pattern-char>
                                            equal cell in Moore neighborhood
      NW             N              NE             W              E              SW             S              SE
```

Argument
|     Name     |         Type         | Description                                                                                |
| :----------: | :------------------: | :----------------------------------------------------------------------------------------- |
| pattern-char | `symbol`<br>`phrase` | maybe set via symbol or keyword `any` therefore, <br> it doesn't matter what the symbol is |

## Setup statement

Syntax
```
setup
    <command>
    ...
end
```

See also [list of commands](commands.md)