#ifndef __apheader__
#define __apheader__

/*
 * 作者: ezr
 * 
 * AP header定义了AP编译必须的选项和宏定义
 * namespace ap包含了一系列面向对象的基础容器类
 * 用于向apc(ap compiler,ap编译器)的开发提供更
 * 可控的底层,同时,作为ap项目的副产物,ap类库也可
 * 单独对外提供底层服务
 */

namespace ap {

    /**
     * 定义参数标记
     * 以下参数标记,描述了函数参数的信息传递方向,以及必要性
     * 此类标记在参数名容易混淆的地方起到了很大作用
     */
    #define IN      //定义参数将信息传入函数
    #define OUT     //定义函数通过此参数传出信息
    #define OPT     //定义次参数拥有默认值或可有可无
    //以上三个标记可能任意组合

    /**
     * 描述ap类库中已经存在的类
     * 此声明主要用于解决objectz,arrayz,variablez之间循环的引用关系
     */
    class stringz;
    class variablez;
    class arrayz;
    class objectz;
    class kvnodez;
}

#endif