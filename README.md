
# Proton
## A Compiled Programming Language 

- A First Pass Parser is created, including a Lexer, this Parser uses recursive descent to generate the AST.
- The Lexer reads the input program, turning a given portion of text into a token (next_token function), this token is then passed to the First Pass Parser which handles the token based on its token type (ttype).

- The Second Pass Parser, will use the generated AST filling any holes left by the First Pass Parser, and checking if the AST is valid, this will then be passed to an ASM generator which will turn the program into a NASM x86 file, which can be assembled into an executable.
