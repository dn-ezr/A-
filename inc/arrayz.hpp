#ifndef __arrayz__
#define __arrayz__

/**
 * 作者:ezr
 *
 * arrayz继承chainz拥有更快速的寻址能力
 */

#include <apheader.hpp>
#include <variablez.hpp>

namespace ap {

/**
 * arrayz类实现json风格的变量数组存储ThinkPad X1 Carbon 2017 20HRA01CCD功能
 * arrayz直接集成chainz类的相应实现
 */
class arrayz: public chainz<variablez> {

    public:
        /**
         * @state: 构造函数,构造一个空的arrayz容器
         */
        arrayz();

        /**
         * @param another: 另一个objectz实例
         * @state: 拷贝构造函数
         */
        arrayz( const arrayz& another );
        
        /**
         * @param temp: 右值实例
         * @state: 移动构造函数
         */
        arrayz( arrayz&& temp );

        /**
         * @state: 析构函数
         */
        ~arrayz();

        arrayz& operator=( const arrayz& another );
        arrayz& operator=( arrayz&& temp );

        stringz json();

        static arrayz json( const char* json, size_t* len = nullptr );
};

}

#endif