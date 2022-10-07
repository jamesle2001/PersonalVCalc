grammar VCalc;

tokens {
    VAR_DECLARATION_TOKEN,
    ASSIGNMENT_TOKEN,
    CONDITIONAL_TOKEN,
    LOOP_TOKEN,
    PRINT_TOKEN,
    GENERATOR_TOKEN,
    FILTER_TOKEN,
    EXPR_TOKEN,
    BLOCK_TOKEN,
    PARENTHESIS_TOKEN,
    INDEX_TOKEN
}

compilationUnit: statement* EOF;

statement: varDeclaration
        | assignment
        | conditional
        | loop
        | print
        ;

varDeclaration: type ID '=' expression ';' ;
assignment: ID '=' expression ';' ;
conditional: IF '(' expression ')' block FI ';' ;
loop: LOOP '(' expression ')' block POOL ';' ;
print: PRINT '(' expression ')' ';' ;
generator: '[' ID IN expression '|' expression ']' ';' ;
filter: '[' ID IN expression '&' expression ']' ';' ;

block: statement* ;

expression: expr ;
expr: 
    '(' expr ')'                      # Parenthesis
    | expr op1='[' expr op2=']'       # Index
    | expr op='..' expr               # Range
    | expr op=('*' | '/') expr        # MulDiv
    | expr op=('+' | '-') expr        # AddSub
    | expr op=('>' | '<') expr        # GreaterThanLessThan
    | expr op=('==' | '!=') expr      # IsEqualIsNotEqual
    | ID                              # IDAtom
    | INTEGER                         # IntegerAtom
    ;
type: INT | VECTOR ;

IF: 'if' ;
FI: 'fi' ;
LOOP: 'loop' ;
POOL: 'pool' ;
INT: 'int' ;
VECTOR: 'vector' ;
IN: 'in' ;
PRINT: 'print' ;

RANGE: '..' ;
ADD: '+' ;
SUB: '-' ;
MUL: '*' ;
DIV: '/' ;
LESSTHAN: '<' ;
GREATERTHAN: '>' ;
ISEQUAL: '==' ;
ISNOTEQUAL: '!=' ;

ID: [a-zA-Z][a-zA-Z0-9]* ;
INTEGER: [0-9]+ ;
// Skip whitespace
WS : [ \t\r\n]+ -> skip ;
