#ifndef __IGP_cpp__
#define __IGP_cpp__

#include "IGP.hpp"

namespace IGP {

formula::formula():
modifier(emodifier::no),category(eformula::em){

}

formula::formula( const formula& another ):
modifier(another.modifier),category(another.category),flat(another.flat),operands(another.operands) {

}

formula::formula( formula&& temp ):
modifier(temp.modifier),category(temp.category),flat(std::move(temp.flat)),operands(std::move(temp.operands)) {
    temp.modifier = emodifier::no;
    temp.category = eformula::em;
}

formula::~formula() {
    modifier = emodifier::no;
    category = eformula::em;
}


pattern::pattern():
binline(false) {

}
pattern::pattern( const pattern& another ):
binline(another.binline),name(another.name),contexts(another.contexts),formulas(another.formulas),ending(another.ending),nextcont(another.nextcont)
{

}
pattern::pattern( pattern&& temp ):
binline(temp.binline),name(std::move(temp.name)),contexts(std::move(temp.contexts)),formulas(std::move(temp.formulas)),ending(std::move(temp.ending)),nextcont(std::move(temp.nextcont))
{

}
pattern::~pattern(){

}


token::token(){

}
token::token( const token& another ):
names(another.names),code(another.code),meaning(another.meaning),column(another.column),line(another.line)
{

}

token::token( token&& temp ):
names(std::move(temp.names)),code(std::move(temp.code)),meaning(std::move(temp.meaning)),column(std::move(temp.column)),line(std::move(temp.line)){

}
token::~token(){

}

IGP::IGP(){

}
IGP::~IGP(){

}


bool lexical_formula( strings& logger ) {

}

bool IGP::lexical_grammar( const string& code, strings& logger ) {
    
    //the first step: clean the environment
    logger.clear();
    source_code = code;
    parser_line = 1;
    parser_column = parser_offset = 0;
    //parser_stage = estage::bim;
    lexical_patterns.clear();

    //the second step: start the parsing.
}

string IGP::lexical_grammar() {

}

bool IGP::lexical_parse( const string& code, string& logger, tokens& voccabulary ) {

}

}
#endif