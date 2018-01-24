#ifndef __variablez__
#define __variablez__

#include <apheader.hpp>
#include <stringz.hpp>

/**
 * 作者:ezr
 * 
 * variablez提高了数据存取的效率
 * 但同时也降低了数据传送的效率
 */

namespace ap {

class variablez {

    public:
        //static const variablez null;

        enum etype {
            T_ARRAY,
            T_BOOLEAN,
            T_NULL,
            T_NUMBER,
            T_OBJECT,
            T_STRING,
        };

    protected:
        union {
            void* pdata;
            double rdata;
            bool bdata;
        };
        etype m_type;

    public:
        variablez();
        variablez( const variablez& another );
        variablez( variablez&& temp );
        variablez( int r );
        variablez( double r );
        variablez( bool b );
        variablez( const arrayz& a );
        variablez( const objectz& o );
        variablez( const stringz& s );
        variablez( const char* s );
        variablez( arrayz&& a );
        variablez( objectz&& o );
        variablez( stringz&& s );
        ~variablez();
        void clear();

        variablez& operator=( const variablez& another );
        variablez& operator=( variablez&& temp );
        variablez& operator=( int r );
        variablez& operator=( double r );
        variablez& operator=( bool b );
        variablez& operator=( const arrayz& a );
        variablez& operator=( const objectz& o );
        variablez& operator=( const stringz& s );
        variablez& operator=( const char* s );
        variablez& operator=( arrayz&& a );
        variablez& operator=( objectz&& o );
        variablez& operator=( stringz&& s );

        operator double&();
        operator bool&();
        operator arrayz&();
        operator objectz&();
        operator stringz&();

        operator const double&()const;
        operator const bool&()const;
        operator const arrayz&()const;
        operator const objectz&()const;
        operator const stringz&()const;

        etype type()const;
        bool null()const;
        stringz json()const;

        kvnodez& operator[]( const stringz& key );
        const kvnodez& operator[]( const stringz& key )const;
        variablez& operator[]( int index );
        const variablez& operator[]( int index )const;

};

}
#endif