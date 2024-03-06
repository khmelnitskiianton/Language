## Common rules

```cpp
- Initializing                   var x;
- Assignment                     var y = (1+3^2)*a - sum(b,c)/4;
- Inc_dec_oper                   i++;
- Condition                      if (a && (b || c)) { ... } else { ... }
- Loop                           while ((x >= y) == 1) {... break; ... continue;}
- Define_function                void foo(var p, var m) { ... return;}
- Call_function                  hello(); foo(x,y) 
```

## Grammar of my language

**Notation:**

- `[]`        - group of elements
- `[[]]`    - optional module 
- `{}`        - sequence of commands
- `+`          - repeat 1+ times
- `*`          - repeat 0+ times
- `<...>`  - symbol sequence

```cpp
Tokens
    N  ::= <token-number>
    V  ::= <token-variable>
    Y  ::= <token-end>
Main Rules
    G  ::= { M [<;>, <}>] }* Y              //General call first
    M  ::= D | I | W | FC | FD | R | E      //Main, Includes everything
Analysis expressions
    E  ::= H { [ <'!='>,<'>='>,<'<='>,<'=='>,<'<'>,<'>'>] H }*  //Calls first, then priorities
    H  ::= X { [ <||>, <+>, <-> ] X }*
    X  ::= P { [ <&&>, <*>, </>, <%> ] P }*
    P  ::= L { [ <^> ] L }*
    L  ::= B { [ <!>, <++>, <--> ] B }*
    B  ::= <(> E <)> | N | V | FC
Analysis actions
    D  ::= T V [[ < = > E]]                                                                     //Initializing + Assignment
    I  ::= <if> <(> E <)> <{> { M [ <;>, <}> ] }* <}> [[ <else> <{> { M [ <;>, <}> ] } * <}> ]] //Condition
    W  ::= <while> <(> E <)> <{> { M [ <;>, <}> ]}* <}>                                         //Loop
    FD ::= T V <(> [[ D* { <,>, D }* ]] <)> <{> { M [ <;>, <}> ] }* <}>                         //Define function
    FC ::= V <(> [[ E* { <,>, E }* ]] <)>                                                       //Call function
    R  ::= [ <break>, <continue>, <return> [[E]] ]                                              //End of function  
    T  ::= [ 'var', 'void' ]                                                                    //Types of vars
```