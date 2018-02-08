#ifndef __gamma_cpp__
#define __gamma_cpp__


#include <gamma.hpp>
#include <exceptz.hpp>

#if defined __apheader__
namespace ap {
#endif

const stringz gamma::context_global("global");

gamma::lexical_formula::lexical_formula():
modifier(no),format(em){

}

gamma::lexical_formula::lexical_formula( const lexical_formula& another ):
modifier(another.modifier),
format(another.format),
flat(another.flat),
operands(another.operands){

}

gamma::lexical_formula::lexical_formula( lexical_formula&& temp ):
modifier(std::move(temp.modifier)),
format(std::move(temp.format)),
flat(std::move(temp.flat)),
operands(std::move(temp.operands)){

}

gamma::lexical_formula::~lexical_formula() {

}

gamma::lexical_pattern::lexical_pattern():
inline_mark(false),
next_context(context_global){

}

gamma::lexical_pattern::lexical_pattern( const lexical_pattern& another ):
inline_mark(another.inline_mark),
whole_name(another.whole_name),
contexts(another.contexts),
formulas(another.formulas),
ending(another.ending),
next_context(another.next_context){

}

gamma::lexical_pattern::lexical_pattern( lexical_pattern&& temp ):
inline_mark(std::move(temp.inline_mark)),
whole_name(std::move(temp.whole_name)),
contexts(std::move(temp.contexts)),
formulas(std::move(temp.formulas)),
ending(std::move(temp.ending)),
next_context(std::move(temp.next_context)){

}

gamma::lexical_pattern::~lexical_pattern(){

}

gamma::token::token():
column(0),
line(0){

}

gamma::token::token( const token& another ):
maped_names(another.maped_names),
source_code(another.source_code),
meaning(another.meaning),
column(another.column),
line(another.line){

}

gamma::token::token( token&& temp ):
maped_names(std::move(temp.maped_names)),
source_code(std::move(temp.source_code)),
meaning(std::move(temp.meaning)),
column(std::move(temp.column)),
line(std::move(temp.line)) {

}

gamma::token::~token(){

}

void gamma::token::clear(){
    maped_names.clear();
    source_code.clear();
    meaning.clear();
    column = line = 0;
}

#define newline '\n'
#define whitespace ' ':case '\t':case '\r'
#define name '-':case '_':case 'a' ... 'z':case 'A' ... 'Z':case '0' ... '9'


int gamma::lexical_grammar( chainz<stringz>& logger ){

    // use the model of Finite State Machine.
    enum class estate {
        bim,        //before the inline-mark
        bname,      //before the pattern name
        iname,      //in the pattern name
        aname,      //after the pattern name,before @ and ':'
        bcontext,   //before the context name,after the @
        icontext,   //in the context name
        acontext,   //after a context name before ',' or ':'
        bformula,   //after the ':' and before any formula.
        aformula,   //after a formula and before another formula or ';' or '='
        bending,    //after the ';' and before the ending formula
        aending,    //after the ending formula
        bnextcont,  //after the '=' and before the name of next context
        inextcont,  //in the name of next context
        anextcont,  //after the name of next context
        end,        //the end of this pattern
        icomment,   //in the comment

    }       state = estate::bim;
    bool    bstay = false;          //this variable tells the Finite State Machine wether we have to stay for a round.
    lexical_pattern pattern;        //this variable is the target of this action.

    //this function is used to swallow one or more characters and switch to another state
    auto step = [&]( estate s, int swallow = 1 ){state = s;offset += swallow;column += swallow;bstay = true;};
    //this function is used to switch to another state but stay at the current character for one round.
    auto jump = [&]( estate s ){state = s;bstay = true;};


    //start to run the finite state machine
    while( state != estate::end ) {

        //check wether the source code was paied off.
        if( offset == source_code.length() ) {
            switch( state ) {
                case estate::bim:
                case estate::aformula:
                case estate::aending:
                case estate::anextcont:
                case estate::icomment:
                    return 0;
                default:
                    logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                    return -1;
            }
        }

        switch( state ) {

            //waitting for '~' or any character of name.
            case estate::bim:{
                switch( source_code[offset] ) {
                    case '#':jump(estate::icomment);break;
                    case newline:line += 1;column = -1;
                    case whitespace:break;
                    case '~':pattern.inline_mark = true;step(estate::bname);break;
                    case name:
                        jump(estate::iname);break;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //waitting for any character of name.
            case estate::bname:{
                switch( source_code[offset]){
                    case name:
                        jump(estate::iname);break;
                    case whitespace:
                        break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //capture all name character and '.'
            case estate::iname:{
                switch( source_code[offset]){
                    case '.':
                        if( source_code[offset-1] == '.' ) {
                            logger << std::move(exceptz("error:%d,%d;missing name.",line,column));
                            return -1;
                        }
                    case name:
                        pattern.whole_name += source_code[offset];break;
                    case whitespace:
                        step(estate::aname);break;
                    case ':':
                        step(estate::bformula);break;
                    case '@':
                        step(estate::bcontext);break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                };
            }break;

            //after the pattern name, waitting for the symbol which indicates the next stage
            case estate::aname:{
                switch(source_code[offset]){
                    case whitespace:break;
                    case '@':step(estate::bcontext);break;
                    case ':':step(estate::bformula);break;
                    case '.':
                        pattern.whole_name += source_code[offset];
                        step(estate::bname);
                        break;
                    case name:
                        logger << std::move(exceptz("error:%d,%d;Pattern names can not be split.",line,column));
                        return -1;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //before context name waitting for any character of name.
            case estate::bcontext:{
                switch( source_code[offset] ) {
                    case whitespace:break;
                    case name:
                        jump(estate::icontext);break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //in the context name captures any character of name
            case estate::icontext:{
                switch( source_code[offset] ) {
                    case name:
                        temp_str += source_code[offset];break;
                    case ',':
                        pattern.contexts << temp_str;
                        temp_str.clear();
                        step(estate::bcontext);
                        break;
                    case ':':
                        pattern.contexts << temp_str;
                        temp_str.clear();
                        step(estate::bformula);
                        break;
                    case whitespace:
                        pattern.contexts << temp_str;
                        temp_str.clear();
                        step(estate::acontext);
                        break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //waitting for ',' or ':'
            case estate::acontext:{
                switch( source_code[offset] ) {
                    case whitespace:break;
                    case ',':step(estate::bcontext);break;
                    case ':':step(estate::bformula);break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //waitting for at least one formula
            case estate::bformula:{
                struct lexical_formula formula;
                switch(source_code[offset]) {
                    case whitespace:break;
                    case '\"':case '\'':case '`':case '[':
                    case '?':case '+':case '*':
                    case name:
                        if( lexical_formula( formula, logger ) ){
                            pattern.formulas << formula;
                            jump(estate::aformula);
                        }
                        else return -1;
                        break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //waitting for another formula or some symbol
            case estate::aformula:{
                struct lexical_formula formula;
                switch(source_code[offset]) {
                    case '#':jump(estate::icomment);break;
                    case whitespace:break;
                    case ';':step(estate::bending);break;
                    case '=':step(estate::bnextcont);break;
                    case '\"':case '\'':case '`':case '[':
                    case '?':case '+':case '*':
                    case name:
                        if( lexical_formula(formula, logger) ){
                            pattern.formulas << formula;
                            jump(estate::aformula);
                        }
                        else return -1;
                        break;
                    case newline:jump(estate::end);break;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //waiting the ending formular
            case estate::bending:{
                struct lexical_formula formula;
                switch(source_code[offset]) {
                    case whitespace:break;
                    case '\"':case '\'':case '`':case '[':
                    case '?':case '+':case '*':
                    case name:
                        if( lexical_formula( formula,logger) ){
                            pattern.ending = formula;
                            jump(estate::aending);
                        }
                        else return -1;
                        break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //end or start the next context
            case estate::aending:{
                switch( source_code[offset] ) {
                    case '#':jump(estate::icomment);break;
                    case whitespace:break;
                    case '=':step(estate::bnextcont);break;
                    case newline:jump(estate::end);break;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //waiting for any character of name
            case estate::bnextcont:{
                switch( source_code[offset] ) {
                    case whitespace:break;
                    case name:
                        jump(estate::inextcont);break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return -1;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //captures any character of name
            case estate::inextcont:{
                switch( source_code[offset] ) {
                    case whitespace:jump(estate::anextcont);break;
                    case name:pattern.next_context += source_code[offset];break;
                    case newline:jump(estate::end);break;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //to be ending , waiting for a new line or '#'
            case estate::anextcont:{
                switch(source_code[offset] ) {
                    case newline:jump(estate::end);break;
                    case whitespace:break;
                    case '#':jump(estate::icomment);break;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return -1;
                }
            }break;

            //in comment, waiting for a new line
            case estate::icomment:{
                if( source_code[offset] == newline ){
                    jump(estate::end);
                }
            }break;
        }

        //now move the pointer
        if( bstay ){
            bstay = false;
        } else {
            offset += 1;
            column += 1;
        }
    }

    //check wether the pattern could be used, and insert it into the patterns.
    if( pattern.whole_name.length() > 0 ) {
        lexical_pattern* sam = nullptr;
        for( lexical_pattern& ptn :lexical_patterns )
            if( ptn.whole_name == pattern.whole_name )
                sam = &ptn;
        if( sam != nullptr ) *sam = pattern;
        else lexical_patterns << pattern;
    }

    //parsed a pattern successfully
    return 1;
}

bool gamma::lexical_formula( struct lexical_formula& formula, chainz<stringz>& logger ) {

}

gamma::gamma(){

}

gamma::~gamma(){

}

bool gamma::lexical_grammar( const stringz& code, chainz<stringz>& logger ) {

    // the first step, to clean the environment of the parser.
    lexical_patterns.clear();
    context = context_global;
    temp_str.clear();
    temp_token.clear();
    offset = column = 0;
    line = 1;
    //load parameters
    source_code = code;
    logger.clear();
    int ret;

    /**
     * start to parser the patterns.
     */
    do {
        //lexical_grammar read one line of source code each time.
        ret = lexical_grammar( logger );
    }while( ret > 0 );

    /**
     * check the grammars to be available.
     */

    //TODO


    return ret >= 0;
}

bool gamma::lexical_parse( const stringz& code, chainz<stringz>& logger, chainz<token>& vocabulary ) {

}





#if defined __apheader__
namespace ap {
#endif

#endif