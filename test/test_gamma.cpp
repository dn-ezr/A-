#include <gamma.hpp>
#include <iostream>

int main( int argc, char **argv ) {

    gamma parser;
    chainz<stringz> logger;
    stringz grammar = R"(
#AP.lexical.grammar for gamma lexical parser

~name : [alpha,'_']
~name : name*[name,numd]

# keyword.module should be the first keyword detected
# module declaration should be in one line.
keyword.module@global : "module" ; ' \t' = def-module

#after the module token, there should be a label of this module
label.@def-module,code : name ; ' \t' = type-module

#after the label of the module you can endup or declar more detail of this module
keyword.model@type-module: "model" ; '; \t'
keyword.entry@type-module : "entry"; '; \t'
keyword.module-end@type-module : ';' = code

#in the environment of "code" there is no keyword named "module" or "model" .etc
#we can use these token freely

keyword.import@code : "import"

)";
    parser.lexical_grammar( grammar, logger );
    for( stringz& s : logger )
        std::cout << s << std::endl;
}