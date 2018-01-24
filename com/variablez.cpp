#ifndef __variablez_cpp__
#define __variablez_cpp__

#include <variablez.hpp>
#include <arrayz.hpp>
#include <objectz.hpp>
#include <exceptz.hpp>
#include <string>

namespace ap {
    variablez::variablez():
    pdata(nullptr),m_type(T_NULL){

    }
    variablez::variablez( const variablez& another ):
    pdata(nullptr),m_type(another.m_type){
        switch( m_type ) {
            case T_ARRAY:
                pdata = (void*)new arrayz( *(arrayz*)another.pdata );
                break;
            case T_BOOLEAN:
                bdata = another.bdata;
                break;
            case T_NUMBER:
                rdata = another.rdata;
                break;
            case T_OBJECT:
                pdata = (void*)new objectz(*(objectz*)another.pdata);
                break;
            case T_STRING:
                pdata = (void*)new stringz(*(stringz*)another.pdata);
                break;
        }
    }
    variablez::variablez( variablez&& temp ):
    pdata(nullptr),m_type(temp.m_type){
        switch( m_type ) {
            case T_ARRAY:
            case T_OBJECT:
            case T_STRING:
                pdata = temp.pdata;
                break;
            case T_BOOLEAN:
                bdata = temp.bdata;
                break;
            case T_NUMBER:
                rdata = temp.rdata;
                break;
        }
        temp.pdata = nullptr;
        temp.m_type = T_NULL;
    }
    variablez::variablez( int r ):
    rdata(r),m_type(T_NUMBER){

    }
    variablez::variablez( double r ):
    rdata(r),m_type(T_NUMBER){

    }
    variablez::variablez( bool b ):
    bdata(b),m_type(T_BOOLEAN){

    }
    variablez::variablez( const arrayz& a ):
    pdata((void*)new arrayz(a)),m_type(T_ARRAY){

    }
    variablez::variablez( const objectz& o ):
    pdata((void*)new objectz(o)),m_type(T_OBJECT){

    }
    variablez::variablez( const stringz& s ):
    pdata((void*)new stringz(s)),m_type(T_STRING){

    }
    variablez::variablez( const char* s ):
    pdata((void*)new stringz(s)),m_type(T_STRING){

    }
    variablez::variablez( arrayz&& a ):
    pdata((void*)new arrayz(a)),m_type(T_ARRAY){

    }
    variablez::variablez( objectz&& o ):
    pdata((void*)new objectz(o)),m_type(T_OBJECT){
        
    }
    variablez::variablez( stringz&& s ):
    pdata((void*)new stringz(s)),m_type(T_STRING){
        
    }
    variablez::~variablez(){
        clear();
    }
    void variablez::clear(){
        if( pdata != nullptr ) switch( m_type ) {
            case T_ARRAY:
                delete (arrayz*)pdata;
                break;
            case T_OBJECT:
                delete (objectz*)pdata;
                break;
            case T_STRING:
                delete (stringz*)pdata;
                break;
        }
        pdata = nullptr;
    }

    variablez& variablez::operator=( const variablez& another ){
        clear();
        m_type = another.m_type;
        switch( m_type ) {
            case T_ARRAY:
                pdata = (void*)new arrayz(*(arrayz*)another.pdata);
                break;
            case T_OBJECT:
                pdata = (void*)new objectz(*(objectz*)another.pdata);
                break;
            case T_STRING:
                pdata = (void*)new stringz(*(stringz*)another.pdata);
                break;
            case T_BOOLEAN:
                bdata = another.bdata;
                break;
            case T_NUMBER:
                rdata = another.rdata;
                break;
        }
    }
    variablez& variablez::operator=( variablez&& temp ){
        clear();
        m_type = temp.m_type;
        switch( m_type ) {
            case T_ARRAY:
            case T_OBJECT:
            case T_STRING:
                pdata = temp.pdata;
                break;
            case T_BOOLEAN:
                bdata = temp.bdata;
                break;
            case T_NUMBER:
                rdata = temp.rdata;
                break;
        }
        temp.pdata = nullptr;
        temp.m_type = T_NULL;
    }
    variablez& variablez::operator=( int r ) {
        if( m_type != T_NUMBER )
            clear();
        m_type = T_NUMBER;
        rdata = r;
    }
    variablez& variablez::operator=( double r ){
        if( m_type != T_NUMBER )
            clear();
        m_type = T_NUMBER;
        rdata = r;
    }
    variablez& variablez::operator=( bool b ){
        if( m_type != T_BOOLEAN )
            clear();
        m_type = T_BOOLEAN;
        bdata = b;
    }
    variablez& variablez::operator=( const arrayz& a ){
        if( m_type != T_ARRAY ) {
            clear();
            pdata = (void*)new arrayz(a);
            m_type = T_ARRAY;
        } else
            *(arrayz*)pdata = a;
    }
    variablez& variablez::operator=( const objectz& o ){
        if( m_type != T_OBJECT ) {
            clear();
            pdata = (void*)new objectz(o);
            m_type = T_OBJECT;
        } else
            *(objectz*)pdata = o;
    }
    variablez& variablez::operator=( const stringz& s ){
        if( m_type != T_STRING ) {
            clear();
            pdata = (void*)new stringz(s);
            m_type = T_STRING;
        } else
            *(stringz*)pdata = s;
    }
    variablez& variablez::operator=( const char* s ){
        if( m_type != T_STRING ) {
            clear();
            pdata = (void*)new stringz(s);
            m_type = T_STRING;
        } else
            *(stringz*)pdata = s;
    }
    variablez& variablez::operator=( arrayz&& a ){
        if( m_type != T_ARRAY ) {
            clear();
            pdata = (void*)new arrayz(a);
            m_type = T_ARRAY;
        } else
            *(arrayz*)pdata = a;
    }
    variablez& variablez::operator=( objectz&& o ){
        if( m_type != T_OBJECT ) {
            clear();
            pdata = (void*)new objectz(o);
            m_type = T_OBJECT;
        } else
            *(objectz*)pdata = o;
    }
    variablez& variablez::operator=( stringz&& s ){
        if( m_type != T_STRING ) {
            clear();
            pdata = (void*)new stringz(s);
            m_type = T_STRING;
        } else
            *(stringz*)pdata = s;
    }

    variablez::operator double&(){
        if( m_type != T_NUMBER )
            throw exceptz( "variablez,operator double&,类型不匹配" );
        return rdata;
    }
    variablez::operator bool&(){
        if( m_type != T_BOOLEAN )
            throw exceptz( "variablez,operator bool&,类型不匹配" );
        return bdata;
    }
    variablez::operator arrayz&(){
        if( m_type != T_ARRAY )
            throw exceptz( "variablez,operator arrayz&,类型不匹配" );
        return *((arrayz*)pdata);
    }
    variablez::operator objectz&(){
        if( m_type != T_OBJECT )
            throw exceptz( "variablez,operator objectz&,类型不匹配" );
        return *((objectz*)pdata);
    }
    variablez::operator stringz&(){
        if( m_type != T_STRING )
            throw exceptz( "variablez,operator stringz&,类型不匹配" );
        return *((stringz*)pdata);
    }

    variablez::operator const double&()const{
        if( m_type != T_NUMBER )
            throw exceptz( "variablez,operator double& const,类型不匹配" );
        return rdata;
    }
    variablez::operator const bool&()const{
        if( m_type != T_BOOLEAN )
            throw exceptz( "variablez,operator bool const&,类型不匹配" );
        return bdata;
    }
    variablez::operator const arrayz&()const{
        if( m_type != T_ARRAY )
            throw exceptz( "variablez,operator arrayz const&,类型不匹配" );
        return *(arrayz*)pdata;
    }
    variablez::operator const objectz&()const{
        if( m_type != T_OBJECT )
            throw exceptz( "variablez,operator objectz const&,类型不匹配" );
        return *(objectz*)pdata;
    }
    variablez::operator const stringz&()const{
        if( m_type != T_STRING )
            throw exceptz( "variablez,operator stringz const&,类型不匹配" );
        return *(stringz*)pdata;
    }

    variablez::etype variablez::type()const{
        return m_type;
    }
    bool variablez::null()const{
        return m_type == T_NULL;
    }
    stringz variablez::json()const{
        switch( m_type ) {
            case T_NULL:
                return "null";
            case T_BOOLEAN:
                return bdata?"true":"false";
            case T_NUMBER:
                return std::to_string(rdata).data();
            case T_ARRAY:
                return ((arrayz*)pdata)->json();
            case T_OBJECT:
                return ((objectz*)pdata)->json();
            case T_STRING:
                return stringz("\"") + ((stringz*)pdata)->json() + "\"";
        }
    }

    kvnodez& variablez::operator[]( const stringz& key ) {
        if( m_type == T_OBJECT )
            return ((objectz*)pdata)->operator[](key);
        else
            throw exceptz("variabjez,operator[](string),类型不匹配");
    }
    const kvnodez& variablez::operator[]( const stringz& key )const {
        if( m_type == T_OBJECT )
            return ((objectz*)pdata)->operator[](key);
        else
            throw exceptz("variabjez,operator[](string)const,类型不匹配");
    }
    variablez& variablez::operator[]( int index ) {
        if( m_type == T_OBJECT )
            return ((arrayz*)pdata)->operator[](index);
        else
            throw exceptz("variabjez,operator[](int),类型不匹配");
    }
    const variablez& variablez::operator[]( int index )const {
        if( m_type == T_OBJECT )
            return ((arrayz*)pdata)->operator[](index);
        else
            throw exceptz("variabjez,operator[](int)const,类型不匹配");
    }
}
#endif