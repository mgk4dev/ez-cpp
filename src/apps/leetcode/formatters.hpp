#pragma once

#include <format>
#include <vector>

template <typename T>
struct std::formatter<std::vector<T>> {
    constexpr auto parse(std::format_parse_context& parse_ctx)
    {
        auto iter = parse_ctx.begin();
        auto get_char = [&]() { return iter != parse_ctx.end() ? *iter : 0; };
        char c = get_char();
        if (c == 0 || c == '}') {
            m_val_fmt.parse(parse_ctx);
            return iter;
        }
        auto get_next_char = [&]() {
            ++iter;
            char vc = get_char();
            if (vc == 0) { throw format_error("Invalid vector format specification"); }
            return vc;
        };
        if (c == 'w') {
            m_lc = get_next_char();
            m_rc = get_next_char();
            ++iter;
        }
        if ((c = get_char()) == 's') {
            m_sep = get_next_char();
            ++iter;
        }
        if ((c = get_char()) == '/' || c == '}') {
            if (c == '/')  // 7
            {
                ++iter;
            }
            parse_ctx.advance_to(iter);
            iter = m_val_fmt.parse(parse_ctx);
        }
        if ((c = get_char()) != 0 && c != '}') {
            throw format_error("Invalid vector format specification");
        }
        return iter;
    }
    auto format(const std::vector<T>& vec, std::format_context& format_ctx) const
    {
        auto pos = format_ctx.out();
        bool need_sep = false;
        for (const auto& val : vec) {
            if (need_sep) {
                *pos++ = m_sep;
                if (m_sep != ' ') { *pos++ = ' '; }
            }
            if (m_lc != '\0') { *pos++ = m_lc; }
            format_ctx.advance_to(pos);
            pos = m_val_fmt.format(val, format_ctx);
            if (m_rc != '\0') { *pos++ = m_rc; }
            need_sep = true;
        }
        return pos;
    }

private:
    char m_lc = '\0';
    char m_rc = '\0';
    char m_sep = ' ';
    std::formatter<T> m_val_fmt;
};
