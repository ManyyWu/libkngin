#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

KNGIN_NAMESPACE_K_BEGIN

class noncopyable {
protected:
    noncopyable  () = default;

    ~noncopyable () = default;

public:
    noncopyable  (const noncopyable &) = delete;

public:
    void
    operator =   (const noncopyable &) = delete;
};

KNGIN_NAMESPACE_K_END

#endif /* _NONCOPYABLE_H_ */
