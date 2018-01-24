#ifndef __arrayz_cpp__
#define __arrayz_cpp__

#include <apheader.hpp>
#include <exceptz.hpp>
#include <arrayz.hpp>
#include <bits/move.h>

namespace ap {
/**
 * @state: 构造函数,构造一个空的arrayz容器
 */
arrayz::arrayz():
chainz(){

}

/**
 * @param another: 另一个objectz实例
 * @state: 拷贝构造函数
 */
arrayz::arrayz( const arrayz& another ):
chainz(another){

}

/**
 * @param temp: 右值实例
 * @state: 移动构造函数
 */
arrayz::arrayz( arrayz&& temp ):
chainz(std::move(temp)){

}

/**
 * @state: 析构函数
 */
arrayz::~arrayz(){
    clear();
}

arrayz& arrayz::operator=( const arrayz& another ){
    chainz::operator=(another);
    return *this;
}
arrayz& arrayz::operator=( arrayz&& temp ){
    chainz::operator=(std::move(temp));
    return *this;
}

stringz arrayz::json(){
    stringz json = "[";
    bool bs = true;
    for( const variablez& x: *this ){
        if( bs ) bs = false;
        else json += ",";
        json += x.json();
    }
    json += "]";
    return std::move(json);
}

arrayz arrayz::json( const char* json, size_t* len ){
    enum es{
        ba,     //[
        ea,     //"]
        bv,     //tfn"{[0123456789-
        ev,     //
        av,     //,]
        aa
    }state = ba;
    bool stay = false;
    int swallow = 0;
    const char *cur = json;
    stringz value;
    arrayz arr;
    auto jump = [&]( es s ){ stay = true;state = s; };
    auto step = [&]( es s, int w=1 ){ state = s; swallow = w; };


    if( cur == nullptr )
        throw exceptz("arrayz,json(static),无效的字符串指针");

    while( state != aa ) {
        switch( state ) {
            case ba:     //[
                if( !isspace(*cur) ) 
                    if( *cur == '[' )
                        step(ea);
                    else
                        throw exceptz("arrayz,json(static),[%d]数组开始之前出现无效字符:%c", cur-json,*cur);
                break;
            case ea:     //"]
                if( !isspace(*cur) )
                    if( *cur == ']' )
                        jump(av);
                    else
                        jump(bv);
                break;
            case bv:     //tfn"{[0123456789-+
                if( !isspace(*cur) ) switch( *cur ) {
                    case 't':
                        if( cur[1] == 'u' and cur[2] == 'r' and cur[3] == 'e' ) {
                            arr.push( true );
                            step(av,4);
                        }else throw exceptz("arrayz,json(static),[%d]无效字符序列", cur-json,*cur);
                        break;
                    case 'f':
                        if( cur[1] == 'a' and cur[2] == 'l' and cur[3] == 's' and cur[4] == 'e' ) {
                            arr.push( false );
                            step(av,4);
                        }else throw exceptz("arrayz,json(static),[%d]无效字符序列",cur-json,*cur);
                        break;
                    case 'n':
                        if( cur[1] == 'u' and cur[2] == 'l' and cur[3] == 'l' ) {
                            arr.push( variablez());
                            step(av,4);
                        }else throw exceptz("arrayz,json(static),[%d]无效字符序列",cur-json);
                        break;
                    case '"':
                        value.clear();
                        step(ev);
                        break;
                    case '{':{
                        size_t l;
                        arr.push( arrayz::json(cur,&l));
                        step(av,l);
                        break;
                    }
                    case '[': {
                        size_t l;
                        arr.push( arrayz::json(cur,&l));
                        step(av,l);
                        break;
                    }
                    case '0' ... '9':case '-':case '+': {
                        char* l;
                        arr.push( strtod(cur,&l) );
                        if( l == cur ) throw exceptz("objectz,json(static(,[%d]无效的字符序列",cur-json);
                        step(av,l-cur);
                        break;
                    }
                    default:
                        throw exceptz("arrayz,json(static),[%d]无效的字符序列",cur-json);
                        break;
                }break;
            case ev:{     //
                value = stringz::json(cur);
                size_t l = value.strlen();
                arr.push( value );
                if( cur[l] != '"' ) throw exceptz("arrayz,json(static),[%d]json意外的结束",cur+l-json);
                step(av,l+1);
                break;
            }
            case av:     //,]
                if( !isspace(*cur) )
                    if( *cur == ',' )
                        step(bv);
                    else if( *cur == ']' )
                        step(aa);
                    else
                        throw exceptz("arrayz,json(static),[%d]json在值之后出现非法字符",cur-json);
        }
        if( swallow > 0 ) {
            cur += swallow;
            swallow = 0;
        } else if( !stay ) {
            cur += 1;
        } else {
            stay = false;
        }
    }

    if( len != nullptr )
        *len = cur-json;
    return std::move(arr);
}
}
#endif