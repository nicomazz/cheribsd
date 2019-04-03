//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <istream>

// basic_istream<charT,traits>& read(char_type* s, streamsize n);

#include <istream>
#include <cassert>

template <class CharT>
struct testbuf
    : public std::basic_streambuf<CharT>
{
    typedef std::basic_string<CharT> string_type;
    typedef std::basic_streambuf<CharT> base;
private:
    string_type str_;
public:

    testbuf() {}
    testbuf(const string_type& str)
        : str_(str)
    {
        base::setg(const_cast<CharT*>(str_.data()),
                   const_cast<CharT*>(str_.data()),
                   const_cast<CharT*>(str_.data()) + str_.size());
    }

    CharT* eback() const {return base::eback();}
    CharT* gptr() const {return base::gptr();}
    CharT* egptr() const {return base::egptr();}
};

int main(int, char**)
{
    {
        testbuf<char> sb(" 123456789");
        std::istream is(&sb);
        char s[5];
        is.read(s, 5);
        assert(!is.eof());
        assert(!is.fail());
        assert(std::string(s, 5) == " 1234");
        assert(is.gcount() == 5);
        is.read(s, 5);
        assert(!is.eof());
        assert(!is.fail());
        assert(std::string(s, 5) == "56789");
        assert(is.gcount() == 5);
        is.read(s, 5);
        assert( is.eof());
        assert( is.fail());
        assert(is.gcount() == 0);
    }
    {
        testbuf<wchar_t> sb(L" 123456789");
        std::wistream is(&sb);
        wchar_t s[5];
        is.read(s, 5);
        assert(!is.eof());
        assert(!is.fail());
        assert(std::wstring(s, 5) == L" 1234");
        assert(is.gcount() == 5);
        is.read(s, 5);
        assert(!is.eof());
        assert(!is.fail());
        assert(std::wstring(s, 5) == L"56789");
        assert(is.gcount() == 5);
        is.read(s, 5);
        assert( is.eof());
        assert( is.fail());
        assert(is.gcount() == 0);
    }

  return 0;
}
