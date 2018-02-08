#ifndef __gamma_cpp__
#define __gamma_cpp__


#include <gamma.hpp>

#if defined __apheader__
namespace ap {
#endif

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
next_context("global"){

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

int gamma::lexical_grammar( chainz<stringz>& logger ){

}

bool gamma::lexical_formula( chainz<stringz>& logger ) {

}

gamma::gamma(){

}

gamma::~gamma(){

}

bool gamma::lexical_grammar( const stringz& code, chainz<stringz>& logger ) {

}

bool lexical_parse( const stringz& code, stringz* logger, chainz<token>& vocabulary ) {

}





#if defined __apheader__
namespace ap {
#endif

#endif