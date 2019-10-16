#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

__NAMESPACE_BEGIN

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

__NAMESPACE_END

#endif /* _NONCOPYABLE_H_ */
