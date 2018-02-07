#ifndef __IGP__
#define __IGP__

/**
 * author: ezr
 * 
 * global definitions of IGP
 */

#include <chainz.hpp>
#include <stringz.hpp>

namespace IGP {

using string    = ap::stringz;
using strings   = ap::chainz<string>;
using sequence  = ap::chainz<formula>;
using sequences = ap::chainz<sequence>;
using tokens    = ap::chainz<token>;
using patterns  = ap::chainz<pattern>;

/**
 * eformula enumerates all type of formulas
 */
enum class eformula {

    st,     //formula-st    "..."
    ch,     //formula-ch    '...'
    nt,     //formula-nt    `...`
    se,     //formula-se    [...]
    in,     //formula-in    [&...]
    su,     //formula-su    [-...]
    re,     //reference     ...
    em,     //empty formula, undefined formula
};

/**
 * emodifier enumerates all type of modifiers
 */
enum class emodifier {

    om,     //midifier-om   +
    an,     //modifier-an   *
    op,     //modifier-op   ?
    no,     //no modifier here
};

/**
 * describe stuffs of formula
 */
struct formula {

    public:
        emodifier   modifier;   //the modifier of this formula
        eformula    category;   //the type of this formula
        string      flat;       //For st,ch,nt,re, flat is the containing
        sequences   operands;   //For se,in,su, operands is the containing
    
    public:
        formula();
        formula( const formula& another );
        formula( formula&& temp );
        ~formula();
};

/**
 * stands for the lexical grammar pattern
 */
struct pattern {

    public:
        bool        binline;    //wether this pattern is inline pattern
        string      name;       //the whole name of this pattern
        strings     contexts;   //the contexts this pattern belongs
        sequence    formulas;   //the contents of this pattern
        formula     ending;     //if available, this formula matchs the first character to be the ending.
        string      nextcont;   //if available, this string will be the next context.
    
    public:
        pattern( );
        pattern( const pattern& another );
        pattern( pattern&& temp );
        ~pattern();
};

/**
 * stands for the terminators
 */
struct token {

    public:
        strings     names;      //all names maped.
        string      code;       //source code.
        string      meaning;    //meaning
        unsigned    column;
        unsigned    line;
    
    public:
        token();
        token( const token& another );
        token( token&& temp );
        ~token();
};

/**
 * class IGP will be the interface for user to use the IGP functions.
 */
class IGP {

    protected:
        /**
         * functional member.
         */
        patterns   lexical_patterns;    //the lexical patterns for the lexical grammar

        /**
         * the following member variables are used by the parser of lexical grammar
         */
        //estage     parser_stage;        //the stage while parsing the lexical grammar
        string     source_code;         //this is the base pointer of the source code
        int        parser_column,parser_line,parser_offset; //these three member make up the source code pointer

    protected:

        /**
         * This method parsers a formula from the source code.
         * This method moves the pointers.
         */
        bool lexical_formula( strings& logger );
    public:
        IGP();
        ~IGP();

        /**
         * @name: lexical_grammar
         * @param code: the source code of the lexical grammar
         * @param logger: this parameter is used to receive logs generated
         * @return bool: wether this action was successfully executed.
         * @statement: to load lexical grammar into IGP instance.
         * ---- make the source code into a safe-parsing character pointer and initial the environment
         */
        bool lexical_grammar( const string& code, strings& logger );

        /**
         * @name: lexical_grammar
         * @return string: the lexical grammar where lexical_patterns stands for.
         * @statement: report the lexical grammar loaded
         */
        string lexical_grammar();

        /**
         * @name: lexical_parse
         * @param code: the source code you want to parser.
         * @param logger: used to receive logs generated
         * @param vocabulary: used to receive the terminator vocabulary table.
         * @return bool: wether this action was successfully executed.
         * @statement: to parser source code to be a token list;
         */
        bool lexical_parse( const string& code, string& logger, tokens& vocabulary );
};

}
#endif