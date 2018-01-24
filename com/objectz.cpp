#ifndef __objectz_cpp__
#define __objectz_cpp__

/**
 * 作者: ezr
 * 
 * objectz的实现
 */

#include <apheader.hpp>
#include <objectz.hpp>
#include <arrayz.hpp>
#include <exceptz.hpp>
#include <functional>
#include <bits/move.h>
#include <stdlib.h>
#include <ctype.h>

namespace ap {

bool objectz::check_key( const stringz& key ) {
    return !key.empty();
}

/**
 * @state: 构造函数,构造一个空的objectz容器
 */
objectz::objectz():
chainz(){

}

/**
 * @param another: 另一个objectz实例
 * @state: 拷贝构造函数
 */
objectz::objectz( const objectz& another ):
chainz(another){

}

/**
 * @param temp: 右值实例
 * @state: 移动拷贝构造函数
 */
objectz::objectz( objectz&& temp ):
chainz(std::move(temp)){

}

/**
 * @state: 析构函数
 */
objectz::~objectz(){
    clear();
}

/**
 * @param another: 另一个objectz容器实例
 * @return: 自身实例
 * @state: 赋值运算符,清空自身内容,克隆实例内容
 */
objectz& objectz::operator=( const objectz& another ){
    clear();
    chainz::operator=(another);
    return *this;
}

/**
 * @param temp: 右值容器实例
 * @return: 自身实例
 * @state: 赋值运算符,清空自身,从右值实例获取资源
 */
objectz& objectz::operator=( objectz&& temp ){
    clear();
    chainz::operator=(std::move(temp));
    return *this;
}

/**
 * @return: 容器内容总量
 * @state: 统计容器容纳的单位总量
 */
size_t objectz::size()const{
    return m_count;
}

/**
 * @state: 清空容器内容
 */
void objectz::clear(){
    chainz::clear();
}

/**
 * @param key: 新成员的键
 * @param value: 新成员的值
 * @return: 自身实例
 * @state: 使用key和value设置一个键值对
 * ---- 若key已存在,则覆盖
 * ---- 若key包含双引号或为空则失败,抛出异常
 */
objectz& objectz::set( const stringz& key, const variablez& value ){
    
    if( !check_key( key ) ) {
        throw exceptz("objectz,set(&),键不合法");
    }

    for( kvnodez& x : *this ) {
        if( x.key == key ) {
            x = value;
            return *this;
        }
    }
    chainz::push( std::move( kvnodez(key, value) ) );
}
objectz& objectz::set( const stringz& key, variablez&& value ){
    if( !check_key( key ) ) {
        throw exceptz("objectz,set(&&),键不合法");
    }

    for( kvnodez& x : *this ) {
        if( x.key == key ) {
            x = std::move(value);
            return *this;
        }
    }
    chainz::push( std::move( kvnodez(key, std::move(value)) ) );
}

/**
 * @param key: 成员的键
 * @return: key对应的成员是否已经被设置
 * @state: 判断键为key的成员是否已经被设置
 * ---- 若键不合法则失败,抛出异常,键的检查标准同set规则
 */
bool objectz::set( const stringz& key )const{
    if( !check_key( key ) ) {
        throw exceptz("objectz,set(const),键不合法");
    }

    for( const kvnodez& x : *this )
        if( x.key == key )
            return true;
    return false;
}

/**
 * @param key: 成员的键
 * @return: 自身实例
 * @state: 删除一个成员
 * ---- 若key不对应任何成员则失败,抛出异常
 */
objectz& objectz::unset( const stringz& key ){
    if( !check_key( key ) ) {
        throw exceptz("objectz,unset,键不合法");
    }

    size_t index = 0;

    for( kvnodez& x : *this )
        if( x.key == key ) {
            chainz::remove(index);
            return *this;
        } else {
            index += 1;
        }
    throw exceptz("objectz,unset,键不对应任何成员");
}

/**
 * @param key: 成员的键
 * @return: 成员的引用
 * @state: 通过key引用一个成员
 * ---- 若key不合法则失败,抛出异常
 * ---- 若成员不存在,则先创建此成员然后返回引用
 */
kvnodez& objectz::operator[]( const stringz& key ){
    if( !check_key( key ) ) {
        throw exceptz("objectz,operator[],键不合法");
    }

    for( kvnodez& x : *this )
        if( x.key == key ){
            return x;
        }
    return chainz::push(std::move(kvnodez(key,variablez()))).operator[](-1);
}

/**
 * @return: 键列表
 * @state: 将所有成员的键提取
 */
chainz<stringz> objectz::keys()const{
    chainz<stringz> keys;
    for( const kvnodez x : *this )
        keys << x.key;
    return std::move(keys);
}

/**
 * @return: 值列表
 * @state: 将所有成员的值提取
 */
chainz<variablez> objectz::values()const{
    chainz<variablez> values;
    for( const kvnodez x : *this )
        values << x;
    return std::move(values);
}

/**
 * @return: 迭代器
 * @state: 从不同环境创造迭代器
 * ---- 用以支持range-based-for
 */
objectz::iterator objectz::begin(){
    return iterator(*this, 0);
}
objectz::citerator objectz::begin()const{
    return citerator(*this,0);
}
objectz::iterator objectz::end(){
    return iterator(*this,m_count);
}
objectz::citerator objectz::end()const{
    return citerator(*this,m_count);
}

/**
 * @param json: json格式的字符串
 * @param len: 解析的json内容的长度
 * @return: 从json生成的objectz
 * @state: 从json生成一个objectz对象
 * ---- 支持json的所有标准语法
 * ---- 若字符串为空或不满足json语法,则失败,抛出异常
 */
objectz objectz::json( const char* json, size_t *len ){
    enum es{
        bo,     //{
        eo,     //"}
        bk,     //"
        ek,     //
        ak,     //:
        bv,     //tfn"{[0123456789-
        ev,     //
        av,     //,}
        ao
    }state = bo;
    bool stay = false;
    int swallow = 0;
    const char *cur = json;
    stringz key;
    stringz value;
    objectz obj;
    auto jump = [&]( es s ){ stay = true;state = s; };
    auto step = [&]( es s, int w=1 ){ state = s; swallow = w; };


    if( cur == nullptr )
        throw exceptz("objectz,json(static),无效的字符串指针");

    while( state != ao ) {
        switch( state ) {
            case bo:     //{
                if( !isspace(*cur) ) 
                    if( *cur == '{' )
                        step(eo);
                    else
                        throw exceptz("objectz,json(static),[%d]对象开始之前出现无效字符:%c", cur-json,*cur);
                break;
            case eo:     //"}
                if( !isspace(*cur) )
                    if( *cur == '}' )
                        jump(av);
                    else if( *cur == '\"' )
                        jump(bk);
                    else
                        throw exceptz("objectz,json(static),[%d]键开始之前出现无效字符:%c", cur-json, *cur);
                break;
            case bk:     //"
                if( !isspace(*cur) )
                    if( *cur == '"' ){
                        key.clear();
                        step(ek);
                    } else
                        throw exceptz("objectz,json(static),[%d]键开始之前出现无效字符:%c",cur-json, *cur);
                break;
            case ek:{     //
                int cc = 0;
                while( cur[cc] != 0 and cur[cc] != '"' ) cc += 1;
                if( cur[cc] == 0 ) throw exceptz("objectz,json(static),[%d]json字符串意外的停止", cur+cc-json);
                key = stringz( cur, cc );
                if( key.empty() ) throw exceptz("objectz,json(static),[%d]json出现了空键",cur+cc-json);
                key = std::move(stringz(cur,cc));
                step(ak,cc+1);
                break;
            }
            case ak:     //:
                if( !isspace(*cur) )
                    if( *cur == ':' )
                        step(bv);
                    else
                        throw exceptz("objectz,json(static),[%d]键之后出现了无效字符:%c",cur-json,*cur);
                break;
            case bv:     //tfn"{[0123456789-+
                if( !isspace(*cur) ) switch( *cur ) {
                    case 't':
                        if( cur[1] == 'u' and cur[2] == 'r' and cur[3] == 'e' ) {
                            obj.set( key, true );
                            step(av,4);
                        }else throw exceptz("objectz,json(static),[%d]无效字符序列", cur-json,*cur);
                        break;
                    case 'f':
                        if( cur[1] == 'a' and cur[2] == 'l' and cur[3] == 's' and cur[4] == 'e' ) {
                            obj.set( key, false );
                            step(av,4);
                        }else throw exceptz("objectz,json(static),[%d]无效字符序列",cur-json,*cur);
                        break;
                    case 'n':
                        if( cur[1] == 'u' and cur[2] == 'l' and cur[3] == 'l' ) {
                            obj.set( key, variablez());
                            step(av,4);
                        }else throw exceptz("objectz,json(static),[%d]无效字符序列",cur-json);
                        break;
                    case '"':
                        value.clear();
                        step(ev);
                        break;
                    case '{':{
                        size_t l;
                        obj.set( key, objectz::json(cur,&l));
                        step(av,l);
                        break;
                    }
                    case '[': {
                        size_t l;
                        obj.set( key, arrayz::json(cur,&l));
                        step(av,l);
                        break;
                    }
                    case '0' ... '9':case '-':case '+': {
                        char* l;
                        obj.set( key, strtod(cur,&l) );
                        if( l == cur ) throw exceptz("objectz,json(static(,[%d]无效的字符序列",cur-json);
                        step(av,l-cur);
                        break;
                    }
                    default:
                        throw exceptz("objectz,json(static),[%d]无效的字符序列",cur-json);
                        break;
                }break;
            case ev:{     //
                value = stringz::json(cur);
                size_t l = value.strlen();
                obj.set( key, value );
                if( cur[l] != '"' ) throw exceptz("objectz,json(static),[%d]json意外的结束",cur+l-json);
                step(av,l+1);
                break;
            }
            case av:     //,}
                if( !isspace(*cur) )
                    if( *cur == ',' )
                        step(bk);
                    else if( *cur == '}' )
                        step(ao);
                    else
                        throw exceptz("objectz,json(static),[%d]json在值之后出现非法字符",cur-json);
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
    return std::move(obj);
}

/**
 * @return: json格式的字符串
 * @state: 从objectz生成json字符串
 */
stringz objectz::json()const{
    stringz json = "{";
    bool bs = true;
    for( const kvnodez& x: *this ) {
        if( bs ) bs = false;
        else json += ',';
        json = json + '"' + x.key + '"' + ':' + x.json();
    }
    json += "}";
    return json;
}
}
#endif