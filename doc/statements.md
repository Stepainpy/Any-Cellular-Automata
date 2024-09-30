# Statements

## File stuct

```
<world statement>
<alias statement>
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

## Alias statement

Syntax
```
alias
    <set statement>
    ...
end
```

Maybe empty

## Set statement

Syntax
```
set <alias-name> <alias-value>
```

Arguments
|    Name     |   Type   | Description |
| :---------: | :------: | :---------- |
| alias-name  | `phrase` | alias name  |
| alias-value | `symbol` | alias value |

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
| rule-name | `phrase` | used rule type, maybe `count` or `pattern` |

## State statement

Syntax  
`count`
```
state <before-char> to <after-char> if
    <may statement>
    ...
end
```
`pattern`
```
state <before-char> to <after-char> if <pattern statement> end
```

Arguments
|    Name     |         Type         | Description                                  |
| :---------: | :------------------: | :------------------------------------------- |
| before-char | `symbol`<br>`phrase` | the symbol that will be changed, maybe alias |
| after-char  | `symbol`<br>`phrase` | The symbol he will become, maybe alias       |

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
|   Name    |         Type         | Description                                         |
| :-------: | :------------------: | :-------------------------------------------------- |
| need-char | `symbol`<br>`phrase` | requiritur symbol or alias                          |
| quantity  |       `number`       | set of maybe count neighboors, <br> set maybe empty |

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
|     Name     |         Type         | Description                                                                                         |
| :----------: | :------------------: | :-------------------------------------------------------------------------------------------------- |
| pattern-char | `symbol`<br>`phrase` | maybe set via symbol or keyword `any` or alias therefore, <br> it doesn't matter what the symbol is |

## Setup statement

Syntax
```
setup
    <command>
    ...
end
```

See also [list of commands](commands.md)