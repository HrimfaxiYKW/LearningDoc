/*****************************************************************************
assert(expr) 宏
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

// trim from start (in place)
static inline void __ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void __rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
static inline void __trim(std::string &s) {
  __ltrim(s);
  __rtrim(s);
}

static inline bool __get_key(const std::string &str, std::size_t &s, std::string &res) {
  if (s >= str.size()) return false;

  std::size_t e;
  uint32_t brackets_num = 0;
  for (e = s; e < str.size(); e++) {
    if (str.at(e) == '(')
      brackets_num++;
    else if (str.at(e) == ')')
      brackets_num--;
    else if (str.at(e) != ',')
      continue;
    else {
      if (brackets_num)
        continue;
      else
        break;
    }
  }

  res.assign(str, s, e - s);
  s = e + 1;
  return true;
}

static inline std::vector<std::string> __get_keys(const std::string &str) {
  std::vector<std::string> res;
  std::string key;
  std::size_t s = 0;
  while (__get_key(str, s, key)) {
    __trim(key);
    if (!key.empty()) res.push_back(key);
  }
  return res;
}

template <typename H1>
void __add_values(std::vector<std::string> &res, H1 &&value) {
  std::ostringstream s;
  s << std::forward<H1>(value);
  res.push_back(s.str());
}

template <typename H1, typename... T>
void __add_values(std::vector<std::string> &res, H1 &&value, T &&... rest) {
  std::ostringstream s;
  s << std::forward<H1>(value);
  res.push_back(s.str());
  __add_values(res, std::forward<T>(rest)...);
}

static inline std::vector<std::string> __get_values() {
  std::vector<std::string> v;
  return v;
}

template <typename... T>
std::vector<std::string> __get_values(T... rest) {
  std::vector<std::string> v;
  __add_values(v, std::forward<T>(rest)...);
  return v;
}

static inline std::string __kv_string(std::vector<std::string> &k, std::vector<std::string> &v) {
  std::string res;
  std::size_t num = k.size() < v.size() ? k.size() : v.size();
  if (num == 0) return res;

  std::ostringstream s;
  s << "(";
  for (std::size_t i = 0; i < num; i++) {
    s << k[i] << "=" << v[i];
    if (i != num - 1) {
      s << ", ";
    }
  }
  s << ")";
  return s.str();
}

#ifndef likely
#define likely(x) __builtin_expect(((x) != 0), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(((x) != 0), 0)
#endif

#define LST_ASSERT(expr, vars2log...)                                                              \
  do {                                                                                             \
    if (unlikely(!(expr))) {                                                                       \
      std::vector<std::string> k = __get_keys(#vars2log);                                          \
      std::vector<std::string> v = __get_values(vars2log);                                         \
      std::string kv_str = __kv_string(k, v);                                                      \
      printf("[assertion] Fail to check assertion '%s'. %s", #expr, kv_str.c_str()); \
    }                                                                                              \
  } while (0)
