#ifndef __objectz__
#define __objectz__

/**
 * 作者: ezr
 * 
 * objectz描述一个json风格的"对象"
 * objectz使用chainz容纳kvnodez集合
 */

#include <apheader.hpp>
#include <kvnodez.hpp>

namespace ap {

/**
 * objectz继承自chainz但不再是一个通用的容器,所以
 * objectz使用保护策略继承
 */
class objectz:protected chainz<kvnodez> {

    public:
        /**
         * 定义常量,和非常量两种迭代器类型,跟chainz中完全相同
         */
        using iterator = b_iterator<chainz,kvnodez>;
        using citerator = b_iterator<const chainz,const kvnodez>;
    


    protected:
        bool static check_key( const stringz& key );

    public:

        /**
         * @state: 构造函数,构造一个空的objectz容器
         */
        objectz();

        /**
         * @param another: 另一个objectz实例
         * @state: 拷贝构造函数
         */
        objectz( const objectz& another );

        /**
         * @param temp: 右值实例
         * @state: 移动构造函数
         */
        objectz( objectz&& temp );

        /**
         * @state: 析构函数
         */
        ~objectz();

        /**
         * @param another: 另一个objectz容器实例
         * @return: 自身实例
         * @state: 赋值运算符,清空自身内容,克隆实例内容
         */
        objectz& operator=( const objectz& another );

        /**
         * @param temp: 右值容器实例
         * @return: 自身实例
         * @state: 赋值运算符,清空自身,从右值实例获取资源
         */
        objectz& operator=( objectz&& temp );

        /**
         * @return: 容器内容总量
         * @state: 统计容器容纳的单位总量
         */
        size_t size()const;

        /**
         * @state: 清空容器内容
         */
        void clear();

        /**
         * @param key: 新成员的键
         * @param value: 新成员的值
         * @return: 自身实例
         * @state: 使用key和value设置一个键值对
         * ---- 若key已存在,则覆盖
         * ---- 若key包含双引号或为空则失败,抛出异常
         */
        objectz& set( const stringz& key, const variablez& value );
        objectz& set( const stringz& key, variablez&& value );
        
        /**
         * @param key: 成员的键
         * @return: key对应的成员是否已经被设置
         * @state: 判断键为key的成员是否已经被设置
         * ---- 若键不合法则失败,抛出异常,键的检查标准同set规则
         */
        bool set( const stringz& key )const;
        
        /**
         * @param key: 成员的键
         * @return: 自身实例
         * @state: 删除一个成员
         * ---- 若key不对应任何成员则失败,抛出异常
         */
        objectz& unset( const stringz& key );

        /**
         * @param key: 成员的键
         * @return: 成员的引用
         * @state: 通过key引用一个成员
         * ---- 若key不合法则失败,抛出异常
         * ---- 若成员不存在,则先创建此成员然后返回引用
         */
        kvnodez& operator[]( const stringz& key );

        /**
         * @return: 键列表
         * @state: 将所有成员的键提取
         */
        chainz<stringz> keys()const;

        /**
         * @return: 值列表
         * @state: 将所有成员的值提取
         */
        chainz<variablez> values()const;

        /**
         * @return: 迭代器
         * @state: 从不同环境创造迭代器
         * ---- 用以支持range-based-for
         */
        iterator begin();
        citerator begin()const;
        iterator end();
        citerator end()const;
        
        /**
         * @param json: json格式的字符串
         * @param len: 解析的json内容的长度
         * @return: 从json生成的objectz
         * @state: 从json生成一个objectz对象
         * ---- 支持json的所有标准语法
         * ---- 若字符串为空或不满足json语法,则失败,抛出异常
         */
        static objectz json( const char* json, size_t *len = nullptr );

        /**
         * @return: json格式的字符串
         * @state: 从objectz生成json字符串
         */
        stringz json()const;
};

}

#endif