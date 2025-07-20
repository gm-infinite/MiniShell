---
applyTo: '**'
---
# The Norm

## Naming Conventions
- A structure's name must start by s_.
- A typedef's name must start by t_.
- A union's name must start by u_.
- An enum's name must start by e_.
- A global's name must start by g_.
- Identifiers, like variables, functions names, user defined types, can only contain lowercases, digits and '_' (snake_case). No capital letters are allowed.
- Files and directories names can only contain lowercases, digits and '_' (snake_case).
- Characters that aren't part of the standard ASCII table are forbidden, except inside litteral strings and chars.
- All identifiers (functions, types, variables, etc.) names should be explicit, or a mnemonic, should be readable in English, with each word separated by an underscore. This applies to macros, filenames and directories as well.
- Using global variables that are not marked const or static is forbidden and is considered a norm error, unless the project explicitly allows them.
- The file must compile. A file that doesn't compile isn't expected to pass the Norm.

## Formatting Rules
- Each function must be at most 25 lines long, not counting the function's own braces.
- Each line must be at most 80 columns wide, comments included. Warning: a tabulation doesn't count as a single column, but as the number of spaces it represents.
- Functions must be separated by an empty line. Comments or preprocessor instructions can be inserted between functions. At least an empty line must exists.
- You must indent your code with 4-char-long tabulations. This is not the same as 4 spaces, we're talking about real tabulations here (ASCII char number 9). Check that your code editor is correctly configured in order to visually get a proper indentation that will be validated by the norminette.
- Blocks within braces must be indented. Braces are alone on their own line, except in declaration of struct, enum, union.
- An empty line must be empty: no spaces or tabulations.
- A line can never end with spaces or tabulations.
- You can never have two consecutive empty lines. You can never have two consecutive spaces.
- Declarations must be at the beginning of a function.
- All variable names must be indented on the same column in their scope. Note: types are already indented by the containing block.
- The asterisks that go with pointers must be stuck to variable names.
- One single variable declaration per line.
- Declaration and an initialisation cannot be on the same line, except for global variables (when allowed), static variables, and constants.
- In a function, you must place an empty line between variable declarations and the remaining of the function. No other empty lines are allowed in a function.
- Only one instruction or control structure per line is allowed. For example: Assignment in a control structure is forbidden, two or multiple assignments on the same line is forbidden, a newline is needed at the end of a control structure, etc.
- An instruction or control structure can be split into multiple lines when needed. The following lines created must be indented compared to the first line, natural spaces will be used to cut the line, and if applies, operators will be at the beginning of the new line and not at the end of the previous one.
- Unless it's the end of a line, each comma or semi-colon must be followed by a space.
- Each operator or operand must be separated by one - and only one - space.
- Each C keyword must be followed by a space, except for keywords for types (suchas int, char, float, etc.), as well as sizeof.
- Control structures (if, while, etc.) must use braces, unless they contain a single instruction on a single line.

## Rules regarding Functions
- A function can take 4 named parameters at most.
- A function that doesn't take arguments must be explicitly prototyped with the word "void" as the argument.
- Parameters in functions' prototypes must be named.
- You can't declare more than 5 variables per function.
- Return of a function has to be between parenthesis, unless the function returns nothing.
- Each function must have a single tabulation between its return type and its name.

## Rules regarding Typedef, struct, enum and union
- As other C keywords, add a space between "struct" and the name when declaring a struct. Same applies to enum and union.
- When declaring a variable of type struct, apply the usual indentation for the name of the variable. Same applies to enum and union.
- Inside the braces of the struct, enum, union, regular indentation rules apply, like any other blocks.
- As other C keywords, add a space after "typedef", and apply regular indentation for the new defined name.
- You must indent all structures' names on the same column for their scope.
- You cannot declare a structure in a .c file.

## Rules regarding Headers - a.k.a include files
-  The allowed elements of a header file are: header inclusions (system or not), declarations, defines, prototypes and macros.
- All includes must be at the beginning of the file.
- You cannot include a C file in a header file or another C file.
- Header files must be protected from double inclusions. If the file is ft_foo.h, its bystander macro is FT_FOO_H.
- Inclusion of unused headers is forbidden.
- Header inclusion can be justified in the .c file and in the .h file itself using comments.

## Rules regarding Macros and Pre-processors
- Preprocessor constants (or #define) you create must be used only for literal and constant values.
- All #define created to bypass the norm and/or obfuscate code are forbidden.
- You can use macros available in standard libraries, only if those ones are allowed in the scope of the given project.
- Multiline macros are forbidden.
- Macro names must be all uppercase.
- You must indent preprocessor directives inside #if, #ifdef or #ifndef blocks.
- Preprocessor instructions are forbidden outside of global scope.

## Forbidden stuff! You're not allowed to use:
- for
- do...while
- switch
- case
- goto
- Ternary operators such as '?'.
- VLAs - Variable Length Arrays.
- Implicit type in variable declarations

## Rules regarding Comments
- Comments cannot be inside function bodies. Comments must be at the end of a line, or on their own line
- Your comments should be in English, and useful.
- A comment cannot justify the creation of a carryall or bad function. A carryall or bad function usually comes with names that are not explicit such as f1, f2, etc. for the function and a, b, c, etc. for the variables names. A function whose only goal is to avoid the norm, without a unique logical purpose, is also considered as a bad function. Please remind that it is desirable to have clear and readable functions that achieve a clear and simple task each. Avoid any code obfuscation techniques, such as the one-liner, etc.

## Rules regarding Files
- You cannot include a .c file in a .c file.
- You cannot have more than 5 function-definitions in a .c file.

## Rules regarding the Makefile, the following rules apply to the Makefiles:
- The $(NAME), clean, fclean, re and all rules are mandatory. The all rule must be the default one and executed when typing just make.
- If the makefile relinks when not necessary, the project will be considered nonfunctional.
- In the case of a multibinary project, in addition to the above rules, you must have a rule for each binary (for example: $(NAME_1), $(NAME_2), etc.). The "all" rule will compile all the binaries, using each binary rule.
- In the case of a project that calls a function from a non-system library (e.g.: libft) that exists along your source code, your makefile must compile this library automatically.
- All source files needed to compile your project must be explicitly named in your Makefile. For example: no "*.c", no "*.o", etc.

## Validating the code according to the norm and the usage of the norminette command
- The Norm is validated by the norminette, which is available in the 42 School's repository.
- The norminette is a tool that checks your code against the Norm rules. It will return errors and warnings if your code does not comply with the Norm.
- You can run the norminette on your code by executing the command `norminette` in your terminal.
- It is recommended to run the norminette frequently during development to catch issues early.
- The norminette will provide detailed feedback on the issues found in your code, including the file name, line number, and a description of the issue.
- You should fix all errors and warnings reported by the norminette before submitting your code.

## Usage of the norminette command
- Runs on the current folder and any subfolder: norminette
- Runs on the given filename(s): norminette filename.[c/h]
- Prevents stopping on various blocking errors: norminette -d
- Outputs all the debug logging: norminette -dd
