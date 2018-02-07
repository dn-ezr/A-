#ifndef __stringz__
#define __stringz__

/**
 * 作者:ezr
 * 
 * stringz是z系列的成员之一
 */

#include <chainz.hpp>
#include <cstring>

#if defined __apheader__
namespace ap {
#elif
#define OUT
#define OPT
#define IN
#endif

class stringz {

    public:
        static const size_t null;

    protected:
        char* m_buf;
        size_t m_count;
    
    protected:
        static void build_next( const stringz& pattern, size_t* next);

    public:
        stringz( const char* s = nullptr, size_t count = null );
        stringz( const stringz& another );
        stringz( stringz&& temp );
        ~stringz();

        stringz& operator=( const stringz& another );
        stringz& operator=( stringz&& temp );

        bool operator==( const stringz& another )const;
        bool operator!=( const stringz& another )const;

        stringz operator+( IN const stringz& str );
        stringz operator+( IN const char ch );
        stringz& operator+=( IN const stringz& str );
        stringz& operator+=( IN const char ch );

        operator char* ();
        operator const char*()const;

        char& operator[]( IN size_t pos );
        const char& operator[]( IN size_t pos )const;

        bool empty()const;
        size_t length()const;
        size_t strlen()const;
        void clear();

        size_t find( IN const stringz& format, IN OPT size_t start = 0 )const;
        size_t find_one( IN const stringz& format, IN OPT size_t start = 0 )const;
        size_t count( IN const stringz& format, IN OPT size_t end = null, IN OPT size_t start = 0 )const;
        stringz& replace( IN const stringz& from, IN const stringz& to, IN OPT size_t start = 0 );
        stringz& replace_all( IN const stringz& from, const stringz& to );
        stringz& insert( IN const stringz& str, size_t offset );
        chainz<stringz> split( IN const stringz& by )const;

        //用json风格对内容中的特殊序列进行转义
        stringz json()const;
        //用json风格解析转义序列,读入一个字符串,双引号会导致解析提前结束
        static stringz json( const stringz& json );
        static stringz json( const char* json );
};

#if defined __apheader__
}
#endif

#endif