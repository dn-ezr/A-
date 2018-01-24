#ifndef __chainz__
#define __chainz__

#include <functional>
#include <apheader.hpp>
#include <cstring>
#include <stdexcept>
#include <iterator>

/**
 * 作者:ezr
 * 
 * chainz使用指针池结构存储数据
 * 用以弥补链式寻址开销大的缺陷
 * 代价则是内存碎片化极为严重
 */

namespace ap {


template<typename T>
class chainz {

    protected:
        /**
         * 定义迭代器类以支持range-based for特性
         * 由于需要对const和非const两种内容实现同一风格的迭代器,迭代器使用模板类声明
         * ---- aC表示chainz的引用的类型
         * ---- aT则表示chainz的内容的类型
         */
        template<typename aC,typename aT>
        struct b_iterator:public std::iterator<std::random_access_iterator_tag,aT,size_t> {

            public:
                //迭代器所对应的容器引用
                aC& r;
                //迭代器当前所指示的位置
                int pos;

            public:
                /**
                 * @param _r: 迭代器岁对用容器的引用
                 * @param p: 迭代器当前所指向的位置,迭代器不对数字进行修正
                 * ---- 这意味着两点:
                 * ---- ---- 1:正数和负数对迭代器来说是不同的两个位置,即使实际上在chainz中指向同一个位置
                 * ---- ---- 2:若在迭代过程中,容器的内容数量有所变化,迭代器的数值不会随之变化
                 * @state: 构造函数
                 */
                b_iterator( aC& _r, int p ):r(_r),pos(p){}
                /**
                 * @param i: 另一个迭代器
                 * @state: 拷贝构造函数
                 */
                b_iterator( const b_iterator& i ):r(i.r),pos(i.pos){}

                /**
                 * @state: 析构函数
                 */
                ~b_iterator(){}

                /**
                 * @return: 返回自身引用
                 * @state: 自曾函数,将迭代器向前(next方向)推移一个单位
                 */
                b_iterator& operator++(){pos += 1;return *this;}

                /**
                 * @return: 返回自身引用
                 * @state: 自减函数,将迭代器向后(prev方向)推移一个单位
                 */
                b_iterator& operator--(){pos -= 1;return *this;}

                /**
                 * @param a: 向前推进单位
                 * @return: 返回产生的新迭代器
                 * @state: 向前推进函数
                 */
                b_iterator operator+( int a ){return b_iterator( r, pos+a );}

                /**
                 * @param s: 向后推进单位
                 * @return: 返回产生的新迭代器
                 * @state: 向后推进函数
                 */
                b_iterator operator-( int s ){return b_iterator( r, pos-s );}

                /**
                 * @param a: 向前推进的单位
                 * @return: 返回自身引用
                 * @state: 向前推进函数
                 */
                b_iterator& operator+=( int a ){pos+=a;return *this;}

                /**
                 * @param s: 向后推进的单位
                 * @return: 返回自身引用
                 * @state: 向后推进函数
                 */
                b_iterator& operator-=( int s ){pos-=s;return *this;}

                /**
                 * @return: 容器中迭代器对应位置上的元素引用
                 * @state: 间接引用方法,若迭代器所指向的位置超出了容器的范围,容器会抛出异常
                 */
                aT& operator*(){return r[pos];}

                /**
                 * @param an: 另一个迭代器
                 * @return: 两迭代器所指容器与位置是否相同
                 * @state: 判断两个迭代器所指是否相同
                 */
                bool operator==( const b_iterator& an ){ return &r == &an.r and pos == an.pos;}

                /**
                 * @param an: 另一个迭代器
                 * @return: 两迭代器所指容器与位置是否不等
                 * @state: 判断两迭代器所指是否不等
                 */
                bool operator!=( const b_iterator& an ){ return !(*this==an);}
        };
    public:
        /**
         * 定义两种迭代器,分别应对通用的实例和常量实例
         */
        using iterator = b_iterator<chainz,T>;
        using citerator = b_iterator<const chainz,const T>;

    protected:

        /**
         * chainz的内容是元素的指针的列表
         * 所以将元素指针视为一种独立的类型
         */
        using R = T*;
        
        R*      m_pool;     //元素指针列表
        size_t  m_count;    //元素总量统计
    
    public:
        /**
         * @state: 构造函数
         */
        chainz():
        m_pool(nullptr),m_count(0) {

        }

        /**
         * @param another: 另一个chainz实例
         * @state: 拷贝构造函数,从另一个实例按照原顺序拷贝每一个元素
         */
        chainz( const chainz& another ):
        m_pool(nullptr),m_count(0) {
            for( int i = 0; i < another.m_count; i++ )
                push( another[i] );
        }

        /**
         * @param temp: 一个右值实例
         * @state: 移动构造函数,从右值实例中抓取系统资源
         */
        chainz( chainz&& temp ):
        m_pool(temp.m_pool),m_count(temp.m_count) {
            temp.m_pool = nullptr;
            temp.m_count = 0;
        }

        /**
         * @state: 析构函数
         */
        ~chainz() {
            clear();
        }

        /**
         * @param another: 另一个chainz实例
         * @return: 自身引用
         * @state: 赋值运算,将另一个chainz实例的快照装入
         * ---- 容器原有数据会被清空
         */
        chainz& operator=( const chainz& another ) {
            clear();
            for( int i = 0; i < another.m_count; i++ )
                push( another[i] );
            return *this;
        }

        /**
         * @param temp: 一个右值实例
         * @return: 自身引用
         * @state: 赋值运算,从右值实例中移动数据
         * ---- 容器原有数据会被清空
         */
        chainz& operator=( chainz&& temp ) {
            clear();
            m_pool = temp.m_pool;
            m_count = temp.m_count;
            temp.m_count = 0;
            temp.m_pool = nullptr;
        }

        /**
         * @return: 容器内容单位总量
         * @state: 统计容器中内容单位的总量
         */
        size_t size()const{
            return m_count;
        }

        /**
         * @param data: 插入数据的实例
         * @param index: 数据插入后,所在的位置
         * @return: 自身实例
         * @state: 插入方法,将data插入到容器中,使得插入后data的位置为index
         * ---- 若index >= 0
         * ---- ---- 则,index的范围是 [0,size()],代表了一个从0开始的正向地址
         * ---- 若index < 0
         * ---- ---- 则,index的范围是 [-1,-1-size()],代表了一个从size()开始的负向地址
         */
        chainz& insert( IN const T& data, IN int index ) {

            //若表为空,则情况单独处理
            if( m_count == 0 ) {
                if( index == 0 or index == -1 ) {
                    m_pool = new R[1];
                    m_pool[0] = new T(data);
                } else {
                    throw std::runtime_error("chainz,insert,索引超出范围");
                }
            
            //若表不为空
            } else {
                //若索引小于0,则先修正索引到正向
                if( index < 0 )
                    index = m_count + 1 + index;
                //若正向索引超出范围,则抛出异常
                if( index > m_count or index < 0 )
                    throw std::runtime_error("chainz,insert,索引超出范围");
                
                //创建新表,将原表中的元素指针映射到新表中,同时插入新的元素的指针
                R* nr = new R[m_count+1];
                memcpy( nr, m_pool, index*sizeof(R) );
                nr[index] = new T(data);
                memcpy( nr+index+1, m_pool+index, (m_count-index)*sizeof(R));

                //删除原表,替换表指针
                delete[] m_pool;
                m_pool = nr;
            }

            //收尾工作
            m_count += 1;
            return *this;
        }

        /**
         * @param index: 欲删除元素的位置
         * @return: 自身实例
         * @state: 删除index位置上的元素
         * ---- 其中,index可以小于零,相应规则同insert方法
         */
        chainz& remove( IN int index ) {
            if( index < 0 )
                index = m_count + 1 + index;
            if( index >= m_count or index < 0 )
                throw std::runtime_error( "chainz,remove,索引超出范围" );
            
            //删除index对应位置上的元素
            delete m_pool[index];

            //若容器只有此元素一个,则执行清空动作
            if( m_count == 1 ) {
                delete[] m_pool;
                m_pool = nullptr;
                m_count = 0;
            
            //否则,应当重新映射指针表到新的表格中
            } else {
                R* nr = new R[m_count-1];
                memcpy( nr, m_pool, index*sizeof(R) );
                memcpy( nr+index, m_pool+index+1, (m_count-index-1)*sizeof(R) );
                delete[] m_pool;
                m_pool = nr;
                m_count -= 1;
            }

            //收尾
            return *this;
        }

        /**
         * @param ref: 数据实例
         * @return: 自身实例
         * @state: 若ref实例在容器内,则删除ref实例
         */
        chainz& remover( IN const T& ref ) {

            //遍历地址表,判断实例是否属于容器
            for( size_t i = 0; i < m_count; i++ ) 
                if( m_pool[i] == &ref )
                    return remove[i];
            
            //若执行到此,说明实例不在容器内
            throw std::runtime_error( "chainz,remover,引用不在链表内" );
        }

        /**
         * @return: 自身实例
         * @state: 清空容器,删除所有内容
         */
        chainz& clear() {
            if( m_count != 0 ) {
                for( size_t i = 0; i < m_count; i++ )
                    delete m_pool[i];
                delete[] m_pool;
                m_pool = nullptr;
                m_count = 0;
            }
            return *this;
        }

        /**
         * @param data: 将要入栈的数据实例
         * @return: 自身实例
         * @state: 将容器视为栈,在栈顶插入新的数据
         */
        chainz& push( IN const T& data ) {
            return insert( data, -1 );
        }

        /**
         * @return: 自身实例
         * @state: 将容器视为栈,从栈顶删除一个单位
         */
        chainz& pop() {
            return remove(-1);
        }

        /**
         * @param data: 用于接收数据的实例
         * @return: 自身实例
         * @state: 将容器视为栈,从栈顶移出一个单位
         */
        chainz& pop( OUT T& data ){
            T* p = get(-1);
            if( p == nullptr )
                throw std::runtime_error( "chainz,pop,索引超出范围" );
            data = *p;
            return pop();
        }

        /**
         * @param data: 即将入队的数据实例
         * @return: 自身实例
         * @state: 将容器视为队列,在队尾插入数据
         */
        chainz& inqueue( IN const T& data ) {
            return insert( data, -1 );
        }
        chainz& operator<<( IN const T& data ) {
            return inqueue(data);
        }

        /**
         * @return: 自身实例
         * @state: 将容器视为队列,从队头删除一个实例
         */
        chainz& outqueue(){
            return remove(0);
        }

        /**
         * @peram data: 用于接受数据的实例
         * @return: 自身实例
         * @state: 将容器视为队列,从队头移出一个单位
         */
        chainz& outqueue( OUT T& data ) {
            T* p = get(0);
            if( p == nullptr )
                throw std::runtime_error( "chainz,outqueue,索引超出范围" );
            data = *p;
            return outqueue();
        }

        /**
         * @param index: 元素的索引
         * @return: 索引位置上,元素的指针
         * @state: 获取index位置上的元素的指针
         * ---- 若index位置上没有数据,则返回空指针
         */
        T* get( IN int index ) {
            if( index < 0 )
                index = m_count + index;    //与插入时不同,引用时容器内容总量不改变,所以不加一
            if( index >= m_count or index < 0 )
                return nullptr;
            else
                return m_pool[index];
        }
        const T* get( IN int index )const {
            if( index < 0 )
                index = m_count + index;
            if( index >= m_count or index < 0 )
                return nullptr;
            else
                return m_pool[index];
        }

        /**
         * @param index: 元素索引
         * @return: 索引位置上,元素的引用
         * @state: 获取index位置上的元素的引用
         * ---- 若index位置上没有数据,则抛出异常
         */
        T& operator[]( IN int index ) {
            T* p = get(index);
            if( p == nullptr )
                throw std::runtime_error( "chainz,operator[],索引超出范围" );
            return *p;
        }
        const T& operator[]( IN int index )const {
            const T* p = get(index);
            if( p == nullptr )
                throw std::runtime_error( "chainz,operator[]const,索引超出范围" );
            return *p;
        }

        /**
         * @param ref: 数据实例的引用
         * @return: ref实例在容器中的位置
         * @state: 若ref实例在容器中,则获取ref实例的索引值
         */
        int index( IN const T& ref ) const {
            for( size_t i = 0; i < m_count; i++ )
                if( &ref == m_pool[i] )
                    return i;
            throw std::runtime_error("chainz,index,引用不在链表内");
        }

        /**
         * @return: 迭代器
         * @state: 从不同环境创造迭代器
         * ---- 用以支持range-based-for
         */
        iterator begin(){
            return iterator(*this, 0);
        }
        citerator begin()const{
            return citerator(*this,0);
        }
        iterator end(){
            return iterator(*this,m_count);
        }
        citerator end()const{
            return citerator(*this,m_count);
        }

        /**
         * @param func: 筛选函数
         * @return: 筛选过的chainz容器
         * @state: 筛选方法
         * ---- func返回true时当前节点被添加到筛选容器中
         * ---- 遍历本容器,通过func筛选出一个新的链表返回之
         */
        chainz operator%( IN std::function<bool(const T&data)> func )const{
            chainz cnz = chainz();
            for( size_t i = 0; i < m_count; i++ )
                func( *(m_pool[i]) ) and cnz << *(m_pool[i]);
            return move(cnz);
        }
        chainz map( IN std::function<bool(const T&data)> func )const {
            return operator%(func);
        }

};


}
#endif