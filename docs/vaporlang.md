# Vapor Lang
This file describes the syntax of the provided compiler. The language is slightly higher level than assembly.

Vapor lang supports named global variables, functions, conditional flow, loops, memory access, and some operators.
Variable declarations and assignments must end in semicolons. Functions and control flow blocks must be bounded by open and close curly braces `{`,`}`.
The compiler is very whitespace sensitive, not in terms of logic (like python), but in terms of parsing.

#### Comments
Comments begin with two forward slashes. Anything on the same line as a comment that comes after the slashes will be ignored by the compiler.

    // Whole line comment
    var test;// Comment after declaration
    var test2;      //After a bunch of whitespace

#### Variables
Variables are declared with `var`. All variables have global scope and must be declared before being used. Variables can only hold values 0 through 255 (a byte). Variables cannot be initialized when declared.

    var xposition;

#### Assignment
Variables can have values stored in them using the assignment operator `=`. The right hand side of the `=` must either be another variable, literal integer, memory access, or complex expression.

    xposition = 5;
    xposition = xposition + 1;

#### Comparison
Variables can be compared for equality `==`, less than `<`, and greater than `>`. 0 indicates false and 1 indicates true.

    isZero = xposition == 0;
    aboveOne = xposition > 1;

#### Other Operators
Other operators include addition `+`, subtraction `-`, bitwise and `&`, bitwise or `|`, bitwise xor `^`, and bitwise not `~`. Compound use of operators is supported except for not `~` and the order of operations is right to left.

    xposition = xposition - 1;
    xposition = xposition & yposition;
    xposition = ~ xposition;

    // xposition = (xposition - (1 + (1 -3)));
    xposition = xposition - 1 + 1 - 3;

#### Functions
Functions are declared with `func <name> {` and must have a closing `}`. Functions implicitly return at `}` and can explicitly return at `return`. Functions can be called before declared. 

    call move;

    func move {
        if xposition > 125 {
            return;
        }
        xposition = xposition + 2;
    }


#### Memory Access
Addressing memory is done with brackets `[`, `]` via segment and offset. The whitespace around the the brackets and comma is necessary.

    [ 0 , index ] = 1;
    [ 0 , index + 1 ] = 2;
    xposition = xposition + [ 0 , index ];

