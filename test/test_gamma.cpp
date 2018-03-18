#include <gamma.hpp>
#include <iostream>

int main( int argc, char **argv ) {

    gamma parser;
    chainz<stringz> logger;
    stringz grammar = R"(
value.string.flat		 : '\'' +[`\\\'`, '\\' '\''] '\''					; [punct,space]
value.string.auto		 : '\"' +[ `\"\\`, '\\' 'abnrtvx\'\"\`' ] '\"'		; [punct,space]
magic:'\`' +[    `\"\\`   ,    '\\'     'abnrtvx\'\"\`'     ]     '\`'
)";
    parser.lexical_grammar( grammar, logger );
    for( stringz& s : logger )
        std::cout << s << std::endl;
}