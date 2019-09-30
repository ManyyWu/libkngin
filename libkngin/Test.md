# Test

## 1. 异常
## ××× 重要的地方catch ×××
## 断言是为了开发者调试时方便定位错误，而不是提供给用户使用
## 线程处理函数必须catch
## 构造函数的异常当场处理，不能外抛

## 2. 内存
## 所有堆内存使用智能指针管理

## To do 
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