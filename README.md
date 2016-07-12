# Vanilla-C

Overview:
----------
this project is based on C programming language and is my class project.

### Language features:
(1)just have main program
(2)data type:integer, character, string, Boolean
(3)if/if-else and while control structures
(4)basic input/output statements: read, print and println

### Lexical Rules:
(1)RESERVED_WORD = bool | char | const | string | int |
                     if |else | while | read | print | println |
                   main | FALSE | TRUE
(2)SPECIAL_SYMBOL = + | - | * | / | % | < | <= | > | >= | == |
                    != |= | ( | ) | { | } | , | ; | ‘ | “
(3)Tokens are defined by the following regular expressions:
	ID = LETTER ( LETTER | DIGIT )*
	NUMBER = DIGIT DIGIT*
	CHAR_CONST = ‘ ( LETTER | DIGIT | SPECIAL_SYMBOL ) ’
	STRING_CONST = “ ( LETTER | DIGIT | SPECIAL_SYMBOL )* ”
	BOOL_VAL = TRUE | FALSE
	LETTER = A | B | C | … | Y | Z | a | b | c | … | y | z
	DIGIT = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
	
