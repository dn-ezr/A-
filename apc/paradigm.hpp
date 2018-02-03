#ifndef __paradigm__
#define __paradigm__

#include <apheader.hpp>
#include <stringz.hpp>
#include <chainz.hpp>

namespace ap {

struct iunit;
typedef chainz<iunit> isequence;

enum class eunit {
    ubi,        //unit of build-in capture pattern,str will be the name of build-in capture pattern
    ust,        //unit of string
    ucs,        //unit of character-selector
    uss,        //unit of sequence-selector
    uin,        //unit of intersection-operator
    usu,        //unit of substraction-operator
};

enum class emodify {
    mno,        //modifier with no effect
    mom,        //modifier for "one or more".
    mnt,        //modifier for "not".
    man,        //modifier for "any"
    mop,        //modifier for "optional"
};

/**
 * structure iunit stands for a unit in sequences
 */
struct iunit {

    public:
        eunit   type;                   //type of this unit, for ust and ucs,str will be the content of string.
                                        //for ubi, str will be the name of build-in capture pattern.
        emodify modify;
        stringz str;
        chainz<isequence> sequences;
};

struct ipattern {

    public:
        bool              binline;      //wether the name is prefixed by '~'
        chainz<stringz>   names;        //name will be separated by '.'
        isequence         target;
        isequence         ending;
};

struct itoken {

    public:
        chainz<stringz> names;
        stringz         code;
};

class iparserL {
    
    protected:
        stringz grammar;
        chainz<ipattern> patterns;

    public:
        iparserL();
        ~iparserL();

        bool grammar( const stringz& _grammar );
        stringz grammar();

        bool parse( const stringz& code, chainz<itoken> vocabulary, chainz<stringz>& logger );

};

class iparserS {

};

class iparser {

};

}
#endif