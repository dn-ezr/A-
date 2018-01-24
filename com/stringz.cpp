#ifndef __stringz_cpp__
#define __stringz_cpp__

#include <apheader.hpp>
#include <stringz.hpp>
#include <exceptz.hpp>
#include <utility>

namespace ap {

const size_t stringz::null = -1;

void stringz::build_next( const stringz& pattern, size_t* next ){  
    int k;
    int m = pattern.strlen();
    next[0] = 0;
    for (int q = 1,k = 0; q < m; ++q){
        while(k > 0 && pattern[q] != pattern[k])
            k = next[k-1];
        if (pattern[q] == pattern[k])
            k++;
        next[q] = k;
    }
} 
stringz::stringz( const char* s , size_t count ){
    if( s == nullptr ) {
        m_buf = nullptr;
        m_count = 0;
    } else {
        if( count == null ) count = ::strlen(s);
        if( count == 0 ) {
            m_buf = nullptr;
            m_count = 0;
        } else {
            m_buf = new char[count+1];
            memcpy( m_buf, s, count );
            m_buf[count] = 0;
            m_count = count;
        }
    }
}
stringz::stringz( const stringz& another ):
m_count(another.m_count){
    m_buf = new char[m_count+1];
    memcpy( m_buf, another.m_buf, m_count );
    m_buf[m_count] = 0;
}
stringz::stringz( stringz&& temp ):
m_buf(temp.m_buf),m_count(temp.m_count) {
    temp.m_buf = nullptr;
    temp.m_count = 0;
}
stringz::~stringz(){
    clear();
}

stringz& stringz::operator=( const stringz& another ){
    clear();
    m_count = another.m_count;
    m_buf = new char[m_count+1];
    memcpy( m_buf, another.m_buf, m_count );
    m_buf[m_count] = 0;
}
stringz& stringz::operator=( stringz&& temp ) {
    clear();
    m_buf = temp.m_buf;
    m_count = temp.m_count;
    temp.m_buf = nullptr;
    temp.m_count = 0;
}

/**
 * 空字符串与任何字符串都不相等
 */
bool stringz::empty()const {
    return m_count == 0;
}
bool stringz::operator==( const stringz& another )const{
    if(
        m_count == 0 or another.m_count == 0 or 
        m_count != another.m_count or 
        memcmp( m_buf, another.m_buf, m_count ) != 0
        )
        return false;
    return true;
}
bool stringz::operator!=( const stringz& another )const{
    return !(*this==another);
}

stringz stringz::operator+( IN const stringz& str ){
    if( str.m_count == 0 )
        return *this;
    else if( m_count == 0 )
        return str;
    else {
        char *buf = new char[m_count+str.m_count+1];
        memcpy( buf, m_buf, m_count );
        memcpy( buf+m_count, str.m_buf, str.m_count );
        buf[m_count+str.m_count] = 0;
        stringz z( buf, m_count+str.m_count );
        delete[] buf;
        return std::move(z);
    }
}
stringz stringz::operator+( IN const char ch ) {
    char *np = new char[m_count+1];
    memcpy( np, m_buf, m_count );
    np[m_count] = ch;
    stringz z(np,m_count+1);
    delete[] np;
    return  std::move(z);
}
stringz& stringz::operator+=( IN const stringz& str ){
    if( str.m_count != 0 ) {
        char *buf = new char[m_count+str.m_count+1];
        memcpy( buf, m_buf, m_count );
        memcpy( buf+m_count, str.m_buf, str.m_count );
        buf[m_count+str.m_count] = 0;
        m_count += str.m_count;
        if( m_buf != nullptr ) delete[] m_buf;
        m_buf = buf;
    }
    return *this;
}
stringz& stringz::operator+=( IN const char ch ) {
    char *np = new char[m_count+2];
    memcpy( np, m_buf, m_count );
    np[m_count] = ch;
    np[m_count+1] = 0;
    if( m_buf != nullptr ) delete[] m_buf;
    m_count += 1;
    m_buf = np;
}

stringz::operator char* (){
    return m_buf;
}
stringz::operator const char*()const{
    return m_buf;
}

char& stringz::operator[]( IN size_t pos ){
    if( pos >= m_count )
        throw exceptz("stringz,operator[],索引超出范围");
    return m_buf[pos];
}
const char& stringz::operator[]( IN size_t pos )const{
    if( pos >= m_count )
        throw exceptz("stringz,operator[],索引超出范围");
    return m_buf[pos];
}

size_t stringz::length()const{
    return m_count;
}
size_t stringz::strlen()const{
    if( m_buf == nullptr )
        return 0;
    else
        return ::strlen(m_buf);
}
void stringz::clear(){
    if( m_count > 0 ) {
        m_count = 0;
        delete[] m_buf;
        m_buf = nullptr;
    }
}

size_t stringz::find( IN const stringz& format, IN OPT size_t start )const{
    if( format.m_count == 0 or m_count == 0 )
        return null;
    //int m;  
    //n = .size();  
    //m = P.size();  
    size_t next[format.strlen()+2];
    build_next( format, next );
    for (size_t i = start, q = 0; i < m_count; ++i)  
    {  
        while (q > 0 && format[q] != operator[](i))  
            q = next[q - 1];  
        if (format[q] == operator[](i))  
            q++;  
        if (q == format.strlen())  
        {  
            //cout << "模式文本的偏移为：" << (i - m + 1) << endl;  
            return i-format.strlen()+1;
            //q = next[q - 1];//寻找下一个匹配  
        }  
    }
    return null;
}
size_t stringz::find_one( IN const stringz& format, IN OPT size_t start )const{
    for( size_t i = start; i < m_count; i++ ){
       for( size_t p = 0; p < format.m_count; p++ ) {

           if( m_buf[i] == format.m_buf[p] )
               return i;
       }
    }
    return null;
}
size_t stringz::count( IN const stringz& format, IN OPT size_t end, IN OPT size_t start )const{
    if( format.m_count == 0 or m_count == 0 )
        return null;
    size_t i = start;
    size_t j = 1;
    size_t n = 0;
    size_t next[format.m_count+2];  
  
    build_next(format, next); 
  
    while( format.m_count+1-j <= m_count-i ) {  
        if( m_buf[i] == format[j-1] ) {
            i += 1;
            j += 1;
  
            //发现匹配结果，将匹配子串的位置，加入结果  
            if(j == format.m_count + 1) {
                n += 1;
                j = next[j];
            }
        } else {  
            j = next[j];  
            if( j == 0 ) {  
                i += 1;  
                j += 1;  
            }
        }
    }
    return n;
}
stringz& stringz::replace( IN const stringz& from, IN const stringz& to, IN OPT size_t start ){
    size_t target = find( from, start );
    if( target == null ) {
        return *this;
    }

    char *vdata = new char[m_count-from.m_count+to.m_count+1];
    vdata[m_count-from.m_count+to.m_count] = 0;
    memcpy( vdata, m_buf, target );
    memcpy( target+vdata, to.m_buf, to.m_count );
    memcpy( target+vdata+to.m_count, m_buf+target+from.m_count, m_count-target-from.m_count );
    delete[] m_buf;
    m_buf = vdata;
    m_count = m_count-from.m_count+to.m_count;
    return *this;
}
stringz& stringz::replace_all( IN const stringz& from, const stringz& to ){
    size_t target = find( from );
    while( target != null ){
        replace( from, to, target );
        target = find( from, target+to.m_count );
    }
    return *this;
}
stringz& stringz::insert( IN const stringz& str, size_t offset ){
    if( offset > m_count )
        throw exceptz("stringz,insert,偏移量超出串范围");
    if( str.m_count != 0 ) {
        char *buf = new char[m_count+str.m_count+1];
        memcpy( buf, m_buf, offset );
        memcpy( buf+offset, str.m_buf, str.m_count );
        memcpy( buf+m_count+str.m_count, m_buf+offset, m_count-offset );
        buf[m_count+str.m_count] = 0;
        if( m_buf != nullptr ) delete[] m_buf;
        m_buf = buf;
        m_count += str.m_count;
    }
    return *this;
}
chainz<stringz> stringz::split( IN const stringz& by )const{
    chainz<stringz> retv;
    size_t last = 0;
    size_t tag = find(by);
    while( tag != null ) {
        if( tag - last > 0 )
            retv << stringz( m_buf+last, tag - last );
        last = tag + by.m_count;
        tag = find( by, tag + by.m_count );
    }
    if( m_count-last > 0 )
        retv << stringz( m_buf+last, m_count-last);
    return std::move(retv);
}

stringz stringz::json()const{
    stringz s(*this);
    
    s.replace_all( "\\", "\\\\" );
    s.replace_all( "\a", "\\a" );
    s.replace_all( "\b", "\\b" );
    s.replace_all( "\f", "\\f" );
    s.replace_all( "\n", "\\n" );
    s.replace_all( "\r", "\\r" );
    s.replace_all( "\t", "\\t" );
    s.replace_all( "\v", "\\v" );
    s.replace_all( "\"", "\\\"" );
    s.replace_all( "\'", "\\\'");

    return std::move(s);
}

stringz stringz::json( const stringz& json ) {
    size_t end = json.strlen();
    if( end == 0 )
        return json;
    
    char *buf = new char[end];
    size_t count = 0;
    bool btr = false;
    memset( buf, 0, end );

    for( size_t i = 0; i < end; i++ ) 
        if( btr ) switch( json[i] ) {
            case 'a': buf[count++] = '\a';break;
            case 'b': buf[count++] = '\b';break;
            case 'f': buf[count++] = '\f';break;
            case 'n': buf[count++] = '\n';break;
            case 'r': buf[count++] = '\r';break;
            case 't': buf[count++] = '\t';break;
            case 'v': buf[count++] = '\v';break;
            case '\\':buf[count++] = '\\';break;
            case '\'':buf[count++] = '\'';break;
            case '\"':buf[count++] = '\"';break;
            default:
                buf[count++] = json[i];
        } else switch( json[i] ) {
            case '\\':
                btr = true;
                break;
            case '"':
                i = end;
                break;
            default:
                buf[count++] = json[i];
                break;
        }
    
    stringz s = stringz( buf, count );
    delete[] buf;
    return std::move(s);
}

stringz stringz::json( const char* json ) {

    if( json == nullptr )
        return stringz();

    size_t end = ::strlen(json);
    if( end == 0 )
        return json;
    
    char *buf = new char[end];
    size_t count = 0;
    bool btr = false;
    memset( buf, 0, end );

    for( size_t i = 0; i < end; i++ ) 
        if( btr ) switch( json[i] ) {
            case 'a': buf[count++] = '\a';break;
            case 'b': buf[count++] = '\b';break;
            case 'f': buf[count++] = '\f';break;
            case 'n': buf[count++] = '\n';break;
            case 'r': buf[count++] = '\r';break;
            case 't': buf[count++] = '\t';break;
            case 'v': buf[count++] = '\v';break;
            case '\\':buf[count++] = '\\';break;
            case '\'':buf[count++] = '\'';break;
            case '\"':buf[count++] = '\"';break;
            default:
                buf[count++] = json[i];
        } else switch( json[i] ) {
            case '\\':
                btr = true;
                break;
            case '"':
                i = end;
                break;
            default:
                buf[count++] = json[i];
                break;
        }
    
    stringz s = stringz( buf, count );
    delete[] buf;
    return std::move(s);
}

}
#endif