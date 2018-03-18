#ifndef __gamma_cpp__
#define __gamma_cpp__


#include <gamma.hpp>
#include <exceptz.hpp>
#include <functional>

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

struct gamma::lexical_formula& gamma::lexical_formula::operator=( const lexical_formula& another ) {
    modifier = another.modifier;
    format = another.format;
    flat = another.flat;
    operands = another.operands;
}

stringz gamma::lexical_formula::to_string() {
    stringz str;

    stringz flat;
    for( size_t o = 0; o < this->flat.length(); o++ ) {
        switch( this->flat[o] ) {
            case '\t':flat += "\\t";break;
            case '\n':flat += "\\n";break;
            case '\v':flat += "\\v";break;
            case '\\':flat += "\\\\";break;
            case '\"':flat += "\\\"";break;
            case '\'':flat += "\\\'";break;
            case '`':flat += "\\`";break;
            default:
                flat += this->flat[o];
        }
    }

    switch( modifier ) {
        case om:str += '+';break;
        case an:str += '*';break;
        case op:str += '?';break;
        case no:break;
    }

    switch( format ) {
        case st:
            str += '"';
            str += flat;
            str += '"';
            break;
        case ch:
            str += '\'';
            str += flat;
            str += '\'';
            break;
        case nt:
            str += '`';
            str += flat;
            str += '`';
            break;
        case re:case bi:
            str += flat;
            break;
        case se:
            str += '[';
            for( auto& operand : operands ){
                for( auto& formula : operand )
                    str += formula.to_string();
                str += ',';
            }
            str[str.length()-1] = ']';
            break;
        case in:
            str += "[&";
            for( auto& operand : operands ){
                for( auto& formula : operand )
                    str += formula.to_string();
                str += ',';
            }
            str[str.length()-1] = ']';
            break;
        case su:
            str += "[-";
            for( auto& operand : operands ){
                for( auto& formula : operand )
                    str += formula.to_string();
                str += ',';
            }
            str[str.length()-1] = ']';
            break;
    }

    return std::move(str);
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

gamma::lexical_pattern& gamma::lexical_pattern::operator=( const lexical_pattern& another ) {
    inline_mark = another.inline_mark;
    whole_name = another.whole_name;
    contexts = another.contexts;
    formulas = another.formulas;
    ending = another.ending;
    next_context = another.next_context;
}

stringz gamma::lexical_pattern::to_string() {
    stringz str;

    if( inline_mark ) str += '~';
    str += whole_name;
    if( contexts.size() > 0 ) {
        str += '@';
        for( auto& context : contexts ) {
            str += context;
            str += ',';
        }
        str[str.length()-1] = ':';
    } else
        str += ':';
    for( auto& formula : formulas )
        str += formula.to_string();
    if( ending.format != lexical_formula::em ){
        str += ';';
        str += ending.to_string();
    }
    if( next_context.length()>0){
        str += '=';
        str += next_context;
    }
    return std::move(str);
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
                        pattern.inline_mark = false;
                        jump(estate::iname);
                        break;
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
                    case '?':case '+':case '*':
                    case '\"':case '\'':case '`':case '[':
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
                        pattern.next_context.clear();
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

    //check the references and replace them with the real pattern formulas.
    std::function<void(struct lexical_formula&)> replace = [&]( struct lexical_formula& formula ) {
        if( formula.format == lexical_formula::re ) {
            formula.operands.clear();
            for( auto& pattern : lexical_patterns )
                if( match_name_mapping( pattern.whole_name, formula.flat ) ) {
                    formula.format = lexical_formula::se;
                    //formula.flat.clear(); //if I clear the flat,  it would not match the second pattern
                    formula.operands << pattern.formulas;
                }
        } else if( 
            formula.format == lexical_formula::se or
            formula.format == lexical_formula::su or
            formula.format == lexical_formula::in )
            for( auto& operand : formula.operands )
                for( auto& formulax : operand )
                    replace(formulax);
    };
    for( auto& formula : pattern.formulas )
        replace(formula);
    
    //check and simplify the formula
    std::function<void(chainz<struct lexical_formula>&)> 
    simplify = [&]( chainz<struct lexical_formula>& formulas ) {
        for( auto& formula : formulas ) switch( formula.format ) {
            case lexical_formula::se:
            case lexical_formula::in:
            case lexical_formula::su:
                for( auto& operand : formula.operands )
                    simplify(operand);
                if( formula.modifier == lexical_formula::no and formula.operands.size() == 1 ) {
                    int index = formulas.index(formula);
                    for( int i = 0; i < formula.operands[0].size(); i++ )
                        formulas.insert( formula.operands[0][i], index+i+1 );
                    formulas.remove(index);
                } else if( formula.operands.size() == 1 and formula.operands[0].size() == 1 ) {
                    lexical_formula::modifier_type t = formula.modifier;
                    formula = formula.operands[0][0];
                    formula.modifier = t;
                }
        }
    };
    simplify(pattern.formulas);

    //check wether the pattern could be used, and insert it into the patterns.
    if( pattern.whole_name.length() > 0 ) {
        lexical_pattern* sam = nullptr;
        for( lexical_pattern& ptn :lexical_patterns )
            if( ptn.whole_name == pattern.whole_name )
                sam = &ptn;
        if( sam != nullptr ) {
            *sam = pattern;
            logger << stringz("replace: ")+pattern.to_string();
        } else {
            lexical_patterns << pattern;
            logger << stringz("generate: ")+pattern.to_string();
        }
    }

    //TODO : check the minimal expression of the pattern to confirm if it could be used.

    //parsed a pattern successfully
    return 1;
}

bool gamma::lexical_formula( struct lexical_formula& formula, chainz<stringz>& logger ) {

    enum class estate {
        bmo,    //before the modifier
        amo,    //after the modifier
        blq,    //before the second character of the left quoter
        alq,    //after the left quoter
        bfu,    //before a formula
        afu,    //after a formula
        arq,    //after the right quoter
    }   state = estate::bmo;
    bool bstay = false; //wether the machine should wait for a round

    chainz<struct lexical_formula> operand; //for some format of formulas

    //this function is used to swallow one or more characters and switch to another state
    auto step = [&]( estate s, int swallow = 1 ){state = s;offset += swallow;column += swallow;bstay = true;};
    //this function is used to switch to another state but stay at the current character for one round.
    auto jump = [&]( estate s ){state = s;bstay = true;};

    //start the machine
    while( state != estate::arq ){

        //check wether the source code have been paid off.
        if( offset == source_code.length() ) {
            return false;
        }

        switch( state ) {

            //wait for a modifier or any start of an formula
            case estate::bmo:{
                switch( source_code[offset] ) {
                    case '?':formula.modifier = lexical_formula::op;step(estate::amo);break;
                    case '+':formula.modifier = lexical_formula::om;step(estate::amo);break;
                    case '*':formula.modifier = lexical_formula::an;step(estate::amo);break;
                    case '"':case '\'':case '`':case '[':
                    case name:formula.modifier = lexical_formula::no;jump(estate::amo);break;
                    case whitespace:break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return false;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return false;
                }
            }break;

            //after the modifier,  waiting for any start of the formula
            case estate::amo:{
                switch( source_code[offset] ) {
                    case whitespace:break;
                    case name:formula.format = lexical_formula::re;jump(estate::alq);break;
                    case '"':formula.format = lexical_formula::st;step(estate::alq);break;
                    case '\'':formula.format = lexical_formula::ch;step(estate::alq);break;
                    case '`':formula.format = lexical_formula::nt;step(estate::alq);break;
                    case '[':step(estate::blq);break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return false;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return false;
                }
            }break;

            //after the '[' character, to make sure what format it is exactly
            case estate::blq:{
                switch( source_code[offset] ) {
                    case '&':formula.format = lexical_formula::in;step(estate::alq);break;
                    case '-':formula.format = lexical_formula::su;step(estate::alq);break;
                    default:formula.format = lexical_formula::se;jump(estate::alq);break;
                }
            }break;

            //the format of formula has been determined, to solve the contents.
            case estate::alq:{
                switch( formula.format ) {

                    case lexical_formula::re: {
                        switch( source_code[offset] ) {
                            case '.':
                            case name:formula.flat += source_code[offset];break;
                            default:
                                if( 
                                    formula.flat == stringz("numb") or 
                                    formula.flat == stringz("numo") or 
                                    formula.flat == stringz("numd") or 
                                    formula.flat == stringz("numh") or 
                                    formula.flat == stringz("lower") or
                                    formula.flat == stringz("upper") or
                                    formula.flat == stringz("alpha") or
                                    formula.flat == stringz("space") or
                                    formula.flat == stringz("punct") or
                                    formula.flat == stringz("any")
                                )formula.format = lexical_formula::bi;
                            jump(estate::arq);break;
                        }
                    }break;
                    case lexical_formula::st:
                    case lexical_formula::ch:
                    case lexical_formula::nt:
                        if( source_code[offset-1] == '\\' and source_code[offset-2] != '\\' ) {
                            switch( source_code[offset] ) {
                                case '\\':formula.flat += '\\';break;
                                case '\"':formula.flat += '\"';break;
                                case '\'':formula.flat += '\'';break;
                                case '`':formula.flat += '`';break;
                                case 'n':formula.flat += '\n';break;
                                case 'r':formula.flat += '\r';break;
                                case 't':formula.flat += '\t';break;
                                default:
                                    logger << std::move(exceptz("error:%d,%d;Invaild escape sequence '\\%c'.",line,column,source_code[offset]));
                                    return false;
                            }break;
                        } else if( 
                            (formula.format == lexical_formula::st and source_code[offset] == '\"')or
                            (formula.format == lexical_formula::ch and source_code[offset] == '\'')or
                            (formula.format == lexical_formula::nt and source_code[offset] == '`' )) {
                            step(estate::arq);
                            break;
                        } else if( source_code[offset] == '\\' ) {
                            break;
                        } else switch( source_code[offset] ) {
                            case newline:
                                logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                                return false;
                            default:
                                formula.flat += source_code[offset];
                                break;
                        }break;
                    case lexical_formula::se:
                    case lexical_formula::in:
                    case lexical_formula::su:
                        jump(estate::bfu);
                        break;
                }
            }break;

            //for formulas have operands
            case estate::bfu:{
                operand.clear();
                switch( source_code[offset] ) {
                    case whitespace:break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return false;
                    case '?':case '+':case '*':
                    case '\"':case '\'':case '`':case '[':
                    case name: {
                        struct lexical_formula fu;
                        if( lexical_formula( fu, logger ) ) {
                            operand << std::move(fu);
                            jump(estate::afu);
                            break;
                        }else {
                            return false;
                        }
                    }break;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return false;
                }
            }break;

            //after a formula wait for another formula or a ']' or a ','
            case estate::afu:{
                switch( source_code[offset] ) {
                    case whitespace:break;
                    case newline:
                        logger << std::move(exceptz("error:%d,%d;Invalid ending of pattern.",line,column));
                        return false;
                    case ',':formula.operands << std::move(operand);step(estate::bfu);break;
                    case ']':formula.operands << std::move(operand);step(estate::arq);break;    //TODO here to check the count of operands
                    case '?':case '+':case '*':
                    case '\"':case '\'':case '`':case '[':
                    case name: {
                        struct lexical_formula fu;
                        if( lexical_formula( fu, logger ) ) {
                            operand << std::move(fu);
                            jump(estate::afu);
                            break;
                        }else {
                            return false;
                        }
                    }break;
                    default:
                        logger << std::move(exceptz("error:%d,%d;Invaild symbol '%c' occurred.",line,column,source_code[offset]));
                        return false;
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

    return true;
}

bool gamma::match_name_mapping( const stringz& whole, const stringz& layer ) {
    chainz<stringz> layers;
    layers << whole;
    for( size_t off = whole.find( "."); off != stringz::null; off = whole.find(".",off+1) )
        layers << std::move(stringz(whole,off));
    for( auto& l : layers )
        if( l == layer )
            return true;
    return false;
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
     * TODO check the grammars to be available.
     */

    return ret >= 0;
}

stringz gamma::lexical_grammar(){
    stringz str;
    for( auto& pattern : lexical_patterns ){
        str += pattern.to_string();
        str += '\n';
    }
    return std::move(str);
}

bool gamma::lexical_parse( const stringz& code, chainz<stringz>& logger, chainz<token>& vocabulary ) {

}

#if defined __apheader__
namespace ap {
#endif

#endif