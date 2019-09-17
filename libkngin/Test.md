# Test
## ××× 主循环中catch new 抛出的异常 ×××
## 断言是为了开发者调试时方便定位错误，而不是提供给用户使用
## 不要随便catch new抛出的异常，new抛出异常应主动减少连接甚至完全清理后主动退出
## To do 
1. 日志目录从配置文件读
2. 日志类型和前缀从配置文件读
3.  字节序

## bak
```
#ifndef _SYNC_QUEUE_H_
#define _SYNC_QUEUE_H_

#include <deque>
#include "define.h"
#include "noncopyable.h"
#include "lock.h"

__NAMESPACE_BEGIN

template <class _T>
class sync_deque : public noncopyable {
public:
    typedef          size_t                         size_type;
    typedef          std::deque<_T>                 deque;
    typedef typename deque::iterator                iter;
    typedef typename deque::const_iterator          citer;
    typedef typename std::reverse_iterator<citer>   criter;
    typedef typename std::reverse_iterator<iter>    riter;

public:
    virtual
    ~sync_deque ()
    {
        m_cond->release();
        m_mutex->release();
        m_deque.clear();
    }

public:
    static deque *
    create      ();

    void
    release     ();

public:
    virtual _T &
    at          (size_type _n);

    virtual const _T &
    at          (size_type _n) const;

    virtual _T &
    front       ();

    virtual const _T &
    front       () const;

    virtual _T &
    back        ();

    virtual const _T &
    back        () const;

    virtual _T &
    operator [] (size_type _n);

    virtual const _T &
    operator [] (size_type _n) const;

public:
    virtual bool
    push_back   (_T &&_v);

    virtual bool
    push_back   (_T &_v);

    virtual bool
    push_front  (_T &&_v);

    virtual bool
    push_front  (_T &_v);

    virtual bool
    pop_back    (_T &&_v);

    virtual bool
    pop_back    (_T &_v);

    virtual bool
    insert      (citer _p, _T&&_v);

    virtual bool
    insert      (citer _p, _T&_v);

    virtual bool
    erase       (citer _p);

    virtual bool
    erase       (citer _f, citer _l);

public:
    virtual bool
    resize      ();

    virtual void
    clear       ();

    virtual void
    shrink      ();

    virtual size_type
    size        () const;

    virtual size_type
    max_size    () const;

    virtual bool
    empty       () const;

public:
    virtual iter
    begin       ();

    virtual citer
    cbegin      () const;

    virtual riter
    rbegin      ();

    virtual criter
    crbegin     () const;

    virtual iter
    end         ();

    virtual citer
    cend        () const;

    virtual riter
    rend        ();

    virtual criter
    crend       () const;

protected:
    std::deque<_T> m_deque;

    mutex *       m_mutex;

    cond *        m_cond;
};

__NAMESPACE_END

#endif /* _SYNC_QUEUE_H_ */
```