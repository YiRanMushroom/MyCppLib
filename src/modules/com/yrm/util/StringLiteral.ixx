export module com.yrm.util.StringLiteral;

import com.yrm.libcpp.std.compat;

export template<size_t N>
class StringLiteral {
public:
    constexpr StringLiteral(const char (&tar)[N]) {
        std::copy(tar, tar + N, str);
    }

    constexpr const char* get() const {
        return str;
    }

    constexpr size_t size() const {
        return N;
    }

    constexpr char operator[](size_t index) const {
        return str[index];
    }

    char str[N];
};

