#ifndef _SYNC_DEQUE_H_
#define _SYNC_DEQUE_H_

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
    virtual const _T &
    front       () const;

    virtual const _T &
    back        () const;

public:
    virtual bool
    push        (_T &&_v);

    virtual bool
    push        (_T &_v);

    virtual bool
    pop         (_T &&_v);

    virtual bool
    pop         (_T &_v);

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

protected:
    std::deque<_T> m_deque;

    mutex *       m_mutex;

    cond *        m_cond;
};

__NAMESPACE_END

#endif /* _SYNC_DEQUE_H_ */