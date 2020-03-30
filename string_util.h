#ifndef STRING_UTIL_H
#define STRING_UTIL_H
#include <string>
#include <array>

template <class T>
struct copy_ptr
{
    copy_ptr() : m_ptr(0) {}
    copy_ptr(T* t) : m_ptr(t) {}
    copy_ptr(copy_ptr const& p) : m_ptr(p.m_ptr ? new T(*p.m_ptr) : 0) {}
    void reset(T* t = 0) { delete m_ptr; m_ptr = t; }
    copy_ptr& operator=(copy_ptr const& p)
    {
        delete m_ptr;
        m_ptr = p.m_ptr ? new T(*p.m_ptr) : 0;
        return *this;
    }
    T* operator->() { return m_ptr; }
    T const* operator->() const { return m_ptr; }
    T& operator*() { return *m_ptr; }
    T const& operator*() const { return *m_ptr; }
    void swap(copy_ptr<T>& p)
    {
        T* tmp = m_ptr;
        m_ptr = p.m_ptr;
        p.m_ptr = tmp;
    }
    operator bool() const { return m_ptr != 0; }
    ~copy_ptr() { delete m_ptr; }
private:
    T* m_ptr;
};

std::array<char, 4 + std::numeric_limits<__int64>::digits10> to_string(__int64 n);

std::string combine_path(std::string const& lhs, std::string const& rhs);

bool is_complete(std::string const& f);

bool is_alpha(char c);

bool is_digit(char c);

bool is_print(char c);

bool is_space(char c);

bool has_parent_path(std::string const& f);

bool is_root_path(std::string const& f);

std::string split_path(std::string const& f);

char const* filename_cstr(char const* f);

bool compare_string(char const* str, int len, std::string const& str2);

char* allocate_string_copy(char const* str);

char to_lower(char c);

bool string_begins_no_case(char const* s1, char const* s2);

bool string_equal_no_case(char const* s1, char const* s2);

#endif // !STRING_UTIL_H

