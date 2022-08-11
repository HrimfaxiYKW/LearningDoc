/*****************************************************************************
转化过程 defer (expr);
auto defer_obj##__COUNTER__ = make_defer([&](){ expr; });
auto defer_obj##__COUNTER__ = scoped_guard_t([&](){ expr; }); // 这一步是创建了一个临时匿名对象
然后将析构函数定义为 ~scoped_guard_t() { f_(); }
这样当当前线程（函数）退出的时候，会自动调用该对象
 *****************************************************************************/
#pragma once
#include <utility>


template <typename F>
struct scoped_guard_t {
  scoped_guard_t(F &&f) : f_(std::forward<F>(f)) {}
  ~scoped_guard_t() { f_(); }
  scoped_guard_t(const scoped_guard_t &) = delete;
  scoped_guard_t(scoped_guard_t &&) = default;  // no need until "-std=c++17"
  scoped_guard_t &operator=(const scoped_guard_t &) = delete;
  scoped_guard_t &operator=(scoped_guard_t &&) = delete;
  F f_;
};

template <typename F>
scoped_guard_t<F> make_defer(F &&f) {
  // required RVO, otherwise the dtor will call twice
  return scoped_guard_t<F>(std::forward<F>(f));
}

#define CAT1(x, y) x##y
#define CAT2(x, y) CAT1(x, y)
#define defer_obj_name CAT2(defer_obj, __COUNTER__)
#define defer(expr) auto defer_obj_name = make_defer([&]() { expr; });

