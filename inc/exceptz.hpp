#ifndef __exceptz__
#define __exceptz__

/**
 * 作者: ezr
 * 
 * exceptz用于为ap类库提供统一规范的异常报告机制
 * exceptz使用类似printf的格式组织字符串,方便ap
 * 库开发过程中异常信息的组织
 */

#include <exception>
#include <stringz.hpp>

namespace ap {

/**
 * exceptz从标准exception类继承任何特性
 */
class exceptz: public std::exception {

    protected:
        stringz wt; //what()时显示的字符串内容

    public:
        /**
         * @param format: 格式描述字符串,描述异常字符串的格式和固定内容
         * @param ...: 满足格式的参数
         * @state: 构造函数,使用类似于printf的格式创建异常信息
         * ---- 支持:
         * ---- ---- %% 百分号
         * ---- ---- %d 32位int
         * ---- ---- %l 64位long
         * ---- ---- %f 64位double
         * ---- ---- %c 8位char
         * ---- ---- %s 字符串char*
         * ---- ---- %b 布尔
         */
        exceptz( const stringz& format, ... )_GLIBCXX_USE_NOEXCEPT;

        
        virtual ~exceptz()_GLIBCXX_USE_NOEXCEPT;
        virtual const char* what() const _GLIBCXX_USE_NOEXCEPT;
};

}
#endif