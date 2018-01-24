#ifndef __kvnodez_cpp__
#define __kvnodez_cpp__

#include <kvnodez.hpp>
#include <bits/move.h>

namespace ap {
    /**
     * @param k: 新的键值对的键
     * @param v: 新的键值对的值
     * @state: 构造函数
     */
    kvnodez::kvnodez( const stringz& k, const variablez& v ):
    key(k),variablez(v){

    }
    kvnodez::kvnodez( const stringz& k, variablez&& v ):
    key(k),variablez(std::move(v)){

    }

    /**
     * @param v: 键值对的新值
     * @return: 自身实例
     * @state: 通过赋值运算,写入变量内容
     */
    kvnodez& kvnodez::operator=(const variablez& v ){
        variablez::operator=(v);
    }
    kvnodez& kvnodez::operator=(variablez&& v ){
        variablez::operator=(std::move(v));
    }
}
#endif