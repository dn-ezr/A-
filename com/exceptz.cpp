#ifndef __exceptz_cpp__
#define __exceptz_cpp__

#include <exceptz.hpp>
#include <stdarg.h>
#include <string>

#if defined __apheader__
namespace ap {
#endif

exceptz::exceptz(const stringz& format, ... )_GLIBCXX_USE_NOEXCEPT:
wt(format) {
    size_t off = 0;
    if( format.length() == 0 ) {
        return;
    }
    va_list args;
    char *z;
    stringz cz = " ";
    va_start( args, format );
    while( true ) {
        off = wt.find( "%", off );
        if( off == stringz::null )
            break;
        switch( wt[off+1] ) {
            case '%':wt.replace("%%", "%", off );off+=1;break;
            case 'd':wt.replace("%d", std::to_string(va_arg(args,int)).data(), off );break;
            case 'l':wt.replace("%l", std::to_string(va_arg(args,long)).data(), off );break;
            case 'f':wt.replace("%f", std::to_string(va_arg(args,double)).data(), off );break;
            case 'c':cz[0] = (char)va_arg(args,int);wt.replace("%c", cz, off );break;
            case 's':z = va_arg(args,char*);wt.replace("%s", z, off );off+=strlen(z);break;
            case 'b':wt.replace("%b", va_arg(args,int)?"true":"false", off );break;
            default:
                off += 1;
        }
    }
    va_end(args);
}

exceptz::~exceptz()_GLIBCXX_USE_NOEXCEPT{

}

const char* exceptz::what() const _GLIBCXX_USE_NOEXCEPT {
    return wt.length()?&wt[0]:nullptr;
}

exceptz::operator stringz&(){
    return wt;
}

exceptz::operator stringz&&(){
    return std::move(wt);
}

#if defined __apheader__
}
#endif

#endif