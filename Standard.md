# Standart of file

File has text format, consists of printed prefix tree and list of variables.
My tree file is named similar to source and has extension `.tree`

## Prefix tree

- The complete bracket sequence. Node `(value left_subtree right_subtree)`
- No subtree: `_`
  
### Value of node

`<type of data> <data>`

Type of data in {1,2,3}

- `1`: Operator, data - code in table for each operator in language
- `2`: Number,   data - number, int/double in format `1.0`
- `3`: Variable, data - index in table of variables
  
### Table of operators

| Name/Priority |  Code  | Type  | Description | Equavalent in C|
|---------------|--------|-------|-------------|-------------|
|FUNC_DEF|10|Binary|Operator of function definition(body).<br/><ul><li>Left node - `FUNC_DEF_HELP`<br/><ul><li>Left Node - *type of ret(var/void)*</li><li>Right Node - *var(name of function)*</li></ul></li><li>Right node - `FUNC_DEF_HELP`<br/><ul><li>Left Node - subtree of *vars-arguments* and `DIVIDER_ARG`</li><li>Right Node - *instructions*</li><ul></li></ul>|`int summ(int a, int b){...}`|
|FUNC_DEF_HELP|11|Binary|Operator of additional node in function definition.<br/><ul><li>Left node - *subtree* </li> <li>Right node - *subtree* </li></ul>|-|
|CALL|12|Binary|Operator of calling function.<br/><ul><li>Left node - *var(name of function)* </li> <li>Right node - subtree of *vars-arguments* and `DIVIDER_ARG`</li></ul>|`summ(a,b);`|
|RETURN|13|Unary|Operator of return in function.<br/><ul><li>Left node - *var(name of ret var)* or `NULL` </li> <li>Right node - `NULL` </li></ul>|`return`|
|DIVIDER|20|Binary|Operator of dividing instruction.<br/><ul><li>Left node - *instruction* </li> <li>Right node - `DIVIDER` or `NULL`|`;`|
|DIVIDER_ARG|21|Binary|Operator of dividing vars in enumeration.<br/><ul><li>Left node - *var(in list)*</li><li>Right node - `DIVIDER_ARG` or `NULL`|`,`|
|EQUAL|22|Binary|Operator of dividing instruction<br/><ul><li>Left node - *instruction*</li><li>Right node - `DIVIDER` or `NULL`|`;`|
|VAR|100|Binary|Operator of init or assignment variables.<br/><ul><li>Left node - `NULL`</li><li>Right node - *subtree:*<br/>`EQUAL`<ul><li>Left Node - *var in assignment*</li><li>Right Node - *expression*</li></ul></li></ul>|`int x;`|
|VOID|101|Empty|Operator for return value of function.<br/>`NULL` `NULL`|`void`|
|IF|102|Binary|Operator of condition.<br/><ul><li>Left node - `expression`</li><li>Right Node - *instructions(if condition is true)* or *subtree:*<br/>`ELSE`<ul><li>Left Node - *instructions(if condition is true)*</li><li>Right Node - *instructions(if condition is false)*</li></ul></li></ul>|`if`|
|WHILE|103|Binary|Operator of loop.<br/><ul><li>Left node - `expression`</li><li>Right Node - *instructions(if condition is true)*</li></ul>|`while`|
|ELSE|104|Binary|Operator of <ul><li>Left Node - *instructions(if condition is true)*</li><li>Right Node - *instructions(if condition is false)*</li></ul>|`else`|
|BREAK|105|Binary|Operator of break in loop.<br/> `NULL NULL`|`break`|
|CONTINUE|106|Binary|Operator of continue in loop.<br/> `NULL NULL`|`continue`|
|ZERO|30|Binary|Operator of expression|`>`|
|ZERO|31|Binary|Operator of expression|`<`|
|ZERO|32|Binary|Operator of expression|`>=`|
|ZERO|33|Binary|Operator of expression|`<=`|
|ZERO|34|Binary|Operator of expression|`!=`|
|ZERO|35|Binary|Operator of expression|`==`|
|FIRST|40|Binary|Operator of expression|`+`|
|FIRST|41|Binary|Operator of expression|`-`|
|FIRST|42|Binary|Operator of expression|`\|\|`|
|SECOND|50|Binary|Operator of expression|`&&`|
|SECOND|51|Binary|Operator of expression|`%`|
|SECOND|52|Binary|Operator of expression|`*`|
|SECOND|53|Binary|Operator of expression|`/`|
|THIRD|60|Binary|Operator of expression|`^`|
|FOURTH|70|Unary|Operator of expression|`++`|
|FOURTH|71|Unary|Operator of expression|`--`|
|FOURTH|72|Unary|Operator of expression|`!`|

## List of variables

After tree its place for list of variables

`<index(begin with 0)>   <name_of_varible>`

*Code:*

```cpp
var x;                   
var y = x+1;
if (1) {
    print(x);
}
```

*Tree:*
```
(1 21 (1 21 (1 21 (1 100  _ (3 0  _  _ )) _ )(1 100  _ (1 20 (3 1  _  _ )(1 40 (3 0  _  _ )(2 1  _  _ )))))(1 102 (2 1  _  _ )(1 21 (1 12 (3 2  _  _ )(3 0  _  _ )) _ )))
0	x
1	y
2	print
```
