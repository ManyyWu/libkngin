#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

KNGIN_NAMESPACE_K_BEGIN

class noncopyable {
protected:
    noncopyable  () = default;

    ~noncopyable () = default;

protected:
    noncopyable  ( const noncopyable& ) = delete;

    noncopyable&
    operator =   ( const noncopyable& ) = delete;

};

KNGIN_NAMESPACE_K_END

#endif /* _NONCOPYABLE_H_ */
