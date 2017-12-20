# Vapor Lang
This file describes the syntax of the provided compiler. The language is slightly higher level than assembly.

Vapor lang supports named global variables, functions, conditional flow, loops, and some operators.
Variable declarations and statements must end in semicolons. Functions can be bounded by open and close curly braces `{`,`}` but flow of execution is controlled with `call` and `return`. 

#### Comments
Comments begin with two forward slashes. Anything on the same line as a comment that comes after the slashes will be ignored by the compiler.

    // Whole line comment
    var test;// Comment after declaration
    var test2;      //After a bunch of whitespace

#### Variables
Variables are declared with `var`. All variables have global scope and must be declared before being used. Variables can only hold values 0 through 255. Variables cannot be initialized when declared.

    var xposition;

#### Assignment
Variables can have values stored in them using the assignment operator `=`. The right hand side of the `=` must either be another variable, literal integer, or complex expression.

    xposition = 5;
    xposition = xposition + 1;

#### Comparison
Variables can be compared for equality `==`, less than `<`, and greater than `>`. 0 indicates false and 1 indicates true.

    atZero = xposition == 0;
    aboveOne = xposition > 1;

#### Other Operators
Other operators include addition `+`, subtraction `-`, bitwise and `&`, bitwise or `|`, bitwise xor `^`, and bitwise not `~`.

    xposition = xposition - 1;
    xposition = xposition & yposition;
    xposition = ~ xposition;

#### Example

    // Player sprite position
    var xposition; // x
    var yposition; // y

    func move {
        xposition = 5;
        yposition = xposition + 2;
        return
    }

    call move;
