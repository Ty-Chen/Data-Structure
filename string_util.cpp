#include "stdafx.h"
#include <array>
#include "string_util.h"

std::string combine_path(std::string const& lhs, std::string const& rhs)
{
    ASSERT(!is_complete(rhs));
    if (lhs.empty() || lhs == ".") return rhs;
    if (rhs.empty() || rhs == ".") return lhs;

#if defined(TORRENT_WINDOWS) || defined(TORRENT_OS2)
#define TORRENT_SEPARATOR "\\"
    bool need_sep = lhs[lhs.size()-1] != '\\' && lhs[lhs.size()-1] != '/';
#else
#define TORRENT_SEPARATOR "/"
    bool need_sep = lhs[lhs.size()-1] != '/';
#endif
    std::string ret;
    int target_size = lhs.size() + rhs.size() + 2;
    ret.resize(target_size);
    target_size = _snprintf(&ret[0], target_size, "%s%s%s", lhs.c_str()
        , (need_sep?TORRENT_SEPARATOR:""), rhs.c_str());
    ret.resize(target_size);
    return ret;
}

// We need well defined results that don't depend on locale
std::array<char, 4 + std::numeric_limits<__int64>::digits10>
to_string(__int64 n)
{
    std::array<char, 4 + std::numeric_limits<__int64>::digits10> ret;
    char *p = &ret.back();
    *p = '\0';
    __int64 un = n;
    // TODO: warning C4146: unary minus operator applied to unsigned type,
    // result still unsigned
    /*un = -un*/
    if (n < 0)  un = -n;
    do {
        *--p = '0' + un % 10;
        un /= 10;
    } while (un);
    if (n < 0) *--p = '-';
    std::memmove(&ret[0], p, &ret.back() - p + 1);
    return ret;
}

bool is_complete(std::string const& f)
{
    if (f.empty())
        return false;

    int i = 0;
    // match the xx:\ or xx:/ form
    while (f[i] && is_alpha(f[i])) ++i;
    if (i < int(f.size()-1) && f[i] == ':' && (f[i+1] == '\\' || f[i+1] == '/'))
        return true;

    // match the \\ form
    if (int(f.size()) >= 2 && f[0] == '\\' && f[1] == '\\')
        return true;
    return false;

}

bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

bool is_print(char c)
{
    return c >= 32 && c < 127;
}

bool is_space(char c)
{
    const static char* ws = " \t\n\r\f\v";
    return bool(std::strchr(ws, c));
}

bool has_parent_path(std::string const& f)
{
    if (f.empty()) return false;
    if (is_root_path(f)) return false;

    int len = f.size() - 1;
    // if the last character is / or \ ignore it
    if (f[len] == '/' || f[len] == '\\') --len;
    while (len >= 0)
    {
        if (f[len] == '/' || f[len] == '\\')
            break;
        --len;
    }

    return len >= 0;
}

bool is_root_path(std::string const& f)
{
    if (f.empty()) return false;

    // match \\ form
    if (f == "\\\\") return true;
    int i = 0;
    // match the xx:\ or xx:/ form
    while (f[i] && is_alpha(f[i])) ++i;
    if (i == int(f.size()-2) && f[i] == ':' && (f[i+1] == '\\' || f[i+1] == '/'))
        return true;
    // match network paths \\computer_name\ form
    if (f.size() > 2 && f[0] == '\\' && f[1] == '\\')
    {
        // we don't care about the last character, since it's OK for it
        // to be a slash or a back slash
        bool found = false;
        for (int i = 2; i < int(f.size()) - 1; ++i)
        {
            if (f[i] != '\\' && f[i] != '/') continue;
            // there is a directory separator in here,
            // i.e. this is not the root
            found = true;
            break;
        }
        if (!found) return true;
    }

    return false;
}

std::string split_path(std::string const& f)
{
    if (f.empty()) return f;

    std::string ret;
    char const* start = f.c_str();
    char const* p = start;
    while (*start != 0)
    {
        while (*p != '/' && *p != '\0' && *p != '\\')
        {
            ++p;
        }
        if (p - start > 0)
        {
            ret.append(start, p - start);
            ret.append(1, '\0');
        }
        if (*p != 0)
        {
            ++p;
        }
        start = p;
    }
    ret.append(1, '\0');
    return ret;
}

char const* filename_cstr(char const* f)
{
    if (f == 0) return f;

    char const* sep = strrchr(f, '/');

    char const* altsep = strrchr(f, '\\');
    if (sep == 0 || altsep > sep) sep = altsep;

    if (sep == 0) return f;
    return sep+1;
}

bool compare_string(char const* str, int len, std::string const& str2)
{
    if (str2.size() != len) return false;
    return memcmp(str2.c_str(), str, len) == 0;
}

char* allocate_string_copy(char const* str)
{
    if (str == 0) return 0;
    int const len = std::strlen(str) + 1;
    char* tmp = static_cast<char*>(std::malloc(len));
    if (tmp == NULL) return 0;
    std::memcpy(tmp, str, len);
    return tmp;
}

char to_lower(char c)
{
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

bool string_begins_no_case(char const* s1, char const* s2)
{
    while (*s1 != 0)
    {
        if (to_lower(*s1) != to_lower(*s2)) return false;
        ++s1;
        ++s2;
    }
    return true;
}

bool string_equal_no_case(char const* s1, char const* s2)
{
    while (to_lower(*s1) == to_lower(*s2))
    {
        if (*s1 == 0) return true;
        ++s1;
        ++s2;
    }
    return false;
}
