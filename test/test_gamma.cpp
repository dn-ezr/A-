#include <gamma.hpp>
#include <iostream>

int main( int argc, char **argv ) {

    gamma parser;
    chainz<stringz> logger;
    stringz grammar = R"(
        real : numd
        real : +real?['.'+real]
        real : ?'+-'real
        real : real?['eE'real]
    )";
    parser.lexical_grammar( grammar, logger );
    for( stringz& s : logger )
        std::cout << s << std::endl;
}