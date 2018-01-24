#ifndef __kvnodez__
#define __kvnodez__

/**
 * 作者: ezr
 * 
 * kvnodez用于在objectz中表示一个键值对
 */

#include <apheader.hpp>
#include <variablez.hpp>

namespace ap {

    /**
     * kvnodez类只比variablez类多一个key成员
     * 所以kvnodez类使用公有约束继承variablez
     */
    class kvnodez:public variablez{

        public:
            const stringz key;  //表示键的字符串
        
        public:

            /**
             * @param k: 新的键值对的键
             * @param v: 新的键值对的值
             * @state: 构造函数
             */
            kvnodez( const stringz& k, const variablez& v );
            kvnodez( const stringz& k, variablez&& v );

            /**
             * @param v: 键值对的新值
             * @return: 自身实例
             * @state: 通过赋值运算,写入变量内容
             */
            kvnodez& operator=(const variablez& v );
            kvnodez& operator=(variablez&& v );
    };
}
#endif