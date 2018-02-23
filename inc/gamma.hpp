#ifndef __gamma__
#define __gamma__

/**
 * author: ezr
 * 
 * gamma is a grammar parser of IGP grammar
 */

#include <chainz.hpp>
#include <stringz.hpp>

#if defined __apheader__
namespace ap {
#endif

/**
 * class gamma will be the interface for user to use the IGP functions.
 */
class gamma {

    public:

        /**
         * describe lexical formula.
         */
        struct lexical_formula {

            public:
                /**
                 * formula_format enumeratres all formats for formula
                 */
                enum formula_format {
                    st,     //formula-st    "..."
                    ch,     //formula-ch    '...'
                    nt,     //formula-nt    `...`
                    se,     //formula-se    [...]
                    in,     //formula-in    [&...]
                    su,     //formula-su    [-...]
                    re,     //reference     ...
                    bi,     //built-in patterns, the flat parameter will be the rule name.
                    em,     //empty formula, undefined formula
                };

                /**
                 * modifier_type enumerates all type of modifiers
                 */
                enum modifier_type {

                    om,     //midifier-om   +
                    an,     //modifier-an   *
                    op,     //modifier-op   ?
                    no,     //no modifier here
                };

            public:
                modifier_type           modifier;   //the modifier of this formula
                formula_format          format;     //the type of this formula
                stringz                 flat;       //For st,ch,nt,re, flat is the containing
                chainz<chainz<lexical_formula>> 
                                        operands;   //For se,in,su, operands is the containing
            
            public:
                lexical_formula();
                lexical_formula( const lexical_formula& another );
                lexical_formula( lexical_formula&& temp );
                ~lexical_formula();

                lexical_formula& operator=( const lexical_formula& another );
                stringz to_string();
        };

        /**
         * stands for the lexical grammar pattern
         */
        struct lexical_pattern {

            public:
                bool                    inline_mark;    //wether this pattern is inline pattern
                stringz                 whole_name;     //the whole name of this pattern
                chainz<stringz>         contexts;       //the contexts this pattern belongs
                chainz<lexical_formula> formulas;       //the contents of this pattern
                lexical_formula         ending;         //if available, this formula matchs the first character to be the ending.
                stringz                 next_context;   //if available, this string will be the next context.

            public:
                lexical_pattern();
                lexical_pattern( const lexical_pattern& another );
                lexical_pattern( lexical_pattern&& temp );
                ~lexical_pattern();

                lexical_pattern& operator=( const lexical_pattern& another );
                stringz to_string();
        };

        /**
         * stands for the terminators
         */
        struct token {

            public:
                chainz<stringz>     maped_names;      //all names maped.
                stringz             source_code;      //source code.
                stringz             meaning;          //meaning
                unsigned            column;
                unsigned            line;
            
            public:
                token();
                token( const token& another );
                token( token&& temp );
                ~token();

                void clear();
        };

    protected:

        const static stringz context_global;

        /**
         * functional members.
         */
        chainz<lexical_pattern>   lexical_patterns;    //the lexical patterns for the lexical grammar

        /**
         * the following member variables are used by the parser of lexical grammar
         */
        stringz    context;            //the current context name.
        stringz    source_code;        //this is the base pointer of the source code
        stringz    temp_str;           //a temp string for parsing.
        token      temp_token;         //a temp token for parsing.
        int        column,line,offset; //these three member make up the source code pointer
    
    protected:
        /**
         * @name : lexical_grammar
         * @param logger : this parameter is used to receive logs generated
         * return int : this return value tells the status of this action.
         * ---- return > 0 : success
         * ---- return = 0 : there is no more patterns.
         * ---- return < 0 : an error occured, the statement will be recorded in the logger.
         * @statement: this method pares one line of the source code and try to generate a pattern from it.
         */
        int lexical_grammar( chainz<stringz>& logger );

        /**
         * @name : lexical_formula
         * @param logger : this parameter is used to receive logs generated
         * @return bool : this return value tells the status of this action
         */
        bool lexical_formula( lexical_formula& formula, chainz<stringz>& logger );

        /**
         * @name : match_name_mapping
         * @param whole : the whole name which will be mapped to layers
         * @param layer : one or several layers of a name
         * @return bool : whether the parameter layer could be contained by the parameter whole.
         */
        static bool match_name_mapping( const stringz& whole, const stringz& layer);
    public:
        gamma();
        ~gamma();

        /**
         * @name: lexical_grammar
         * @param code: the source code of the lexical grammar
         * @param logger: this parameter is used to receive logs generated
         * @return bool: wether this action was successfully executed.
         * @statement: to load lexical grammar into gamma instance.
         * ---- make the source code into a safe-parsing character pointer and initial the environment
         */
        bool lexical_grammar( const stringz& code, chainz<stringz>& logger );

        /**
         * @name: lexical_grammar
         * @return stringz: the lexical grammars the lexical_patterns stands for
         * @statement: get the grammar codes, this function used to debug.
         */
        stringz lexical_grammar();

        /**
         * @name: lexical_parse
         * @param code: the source code you want to parser.
         * @param logger: used to receive logs generated
         * @param vocabulary: used to receive the terminator vocabulary table.
         * @return bool: wether this action was successfully executed.
         * @statement: to parser source code to be a token list;
         */
        bool lexical_parse( const stringz& code, chainz<stringz>& logger, chainz<token>& vocabulary );
};

#if defined __apheader__
}
#endif

#endif