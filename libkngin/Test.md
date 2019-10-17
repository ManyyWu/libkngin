# Test

### 1. 异常
## ××× 重要的地方catch ×××
## 断言是为了开发者调试时方便定位错误，而不是提供给用户使用
## 线程处理函数必须catch
## 构造函数的异常当场处理，不能外抛，构造函数失败必须抛出异常
## ------------------------------------------------
## C :: C()
## try 
##   ：A(/*...*/) //可选的初始化列表b
##   (/*...*/)
## { 
## } 
## catch(...)
## { 
##   // auto throw out the exception that catched
## }
## 如果处理程序主体包含语句“ throw;”;那么catch块显然会抛出
## A::A()或B::B()异常。在标准中不太明显但明确指出的是，如果catch块没有抛出（要么抛出原始异常，要么抛出新的东西），
## 并且控制到达构造函数或析构函数的catch块的末尾，则原始异常会自动重新抛出。
## ------------------------------------------------

### 2. 内存
## 所有堆内存使用智能指针管理

### To do 
1. 日志目录从配置文件读
2. 日志类型和前缀从配置文件读, 不同worker使用不同文件名
3. 字节序
4. new delete记录位置，数量，计数，未释放数,是否是数组 __ENABLE_MEMORY_LEAK_CHECK, memory.log
// 定义new_nothrow(出错先assert，然后log)，返回NULL时return E_NOMEM
// 调用依赖库定义的函数出错先assert，然后log，最后return E_XXXX assert_log
// 函数入口检查所有参数
// 接口封装尽量不爆露api(平台特性，返回值), 错误码记录到日志，返回bool或E_XXXX
// assert改kassert
// assert if 改if_not
// 日志的目的是精确记录出错位置及出错信息，不要记录冗余日志，一般出错时使用if_not，goto fail，在fail中log并记录出错返回值
// kassert_r kassert_r0 必须保证能释放作用域内所有堆上分存被释放******************
// 返回值不使用及量不使用引用类型
// 非正常错误使用if_not 代替if

    /*
    #pragma pack(push, 1)
    template <class __T>
    class ret_val {
    private:
        ret_val  ()                                            {}

    public:
        ret_val  (__T _val) : m_code(E_SUCCESS), m_value(_val) {}

        ret_val  (const ret_val &_val) : m_code(_val.m_code)   {}

        ~ret_val ()                                            {}

    public:
        __T
        value             () const         { return m_value; }

        ret_val &
        operator =        (__T _val)       { m_value = _val; return *this; }

    public:
        err_code
        code              ()               { return m_code;}

        void
        clear             ()               { m_code = E_SUCCESS; }

        void
        set_code          (err_code _code) { m_code = _code; }

        explicit
        operator bool     ()               { return m_code; }

        explicit
        operator err_code ()               { return m_code; }

        const char *
        what              ()               { return __err_code_entry[m_code]._str; }

    protected:
        err_code         m_code;

        __T              m_value;
    };
    #pragma pack(pop)

    typedef class ret_val<bool>      ret_bool;
    typedef class ret_val<uint8_t>   ret_uint8;
    typedef class ret_val<int8_t>    ret_int8;
    typedef class ret_val<uint16_t>  ret_uint16;
    typedef class ret_val<int16_t>   ret_int16;
    typedef class ret_val<uint32_t>  ret_uint32;
    typedef class ret_val<int32_t>   ret_int32;
    typedef class ret_val<uint64_t>  ret_uint64;
    typedef class ret_val<int64_t>   ret_int64;
    typedef class ret_val<size_t>    ret_size;
    typedef class ret_val<ssize_t>   ret_ssize;
    typedef class ret_val<timestamp> ret_timestamp;
    typedef class ret_val<uintptr_t> ret_uintptr_t;
    typedef class ret_val<intptr_t>  ret_intptr_t;
    */
