#ifndef KNGIN_NONCOPYABLE_H
#define KNGIN_NONCOPYABLE_H

KNGIN_NAMESPACE_K_BEGIN

class noncopyable {
protected:
  noncopyable  () = default;

  ~noncopyable () = default;

public:
  noncopyable  (const noncopyable&) = delete;

  noncopyable&
  operator =   (const noncopyable&) = delete;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_NONCOPYABLE_H */
