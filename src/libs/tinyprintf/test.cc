#include <string>
#include "printf-c.cc"

static const char flags[][6] = {
    "",
    "-", // leftalign
    "0", // zeropad
    "+", // sign
    " ", // space for positive sign
    "#", // alt format
    "-0", "-+", "- ", "-#",
    "0+", "0 ", "0#",
    "+ ", " #",
    "-0+", "-0 ", "-0#",
    "-+ ", "- #",
    "0+ ", "0+#", "+ #",
    "-0+ ", "-0+#", "-+ #", "0+ #", "-0+ #",

    "--", "00", "++", "  ", "##",
    "-00", "-++", "- - ", "0  -#", "--+# ", "--0",
};

static void PrintParams() { }
template<typename... Params> static void PrintParams(int arg, Params... rest);
template<typename... Params> static void PrintParams(long arg, Params... rest);
template<typename... Params> static void PrintParams(long long arg, Params... rest);
template<typename... Params> static void PrintParams(long double arg, Params... rest);
template<typename... Params> static void PrintParams(double arg, Params... rest);
template<typename... Params> static void PrintParams(const void* arg, Params... rest);
template<typename... Params> static void PrintParams(const char* arg, Params... rest);

template<typename... Params>
static void PrintParams(int arg, Params... rest)
{
    std::printf(", %d", arg);
    PrintParams(rest...);
}
template<typename... Params>
static void PrintParams(long arg, Params... rest)
{
    std::printf(", %ldl", arg);
    PrintParams(rest...);
}
template<typename... Params>
static void PrintParams(long long arg, Params... rest)
{
    std::printf(", %lldll", arg);
    PrintParams(rest...);
}
template<typename... Params>
static void PrintParams(long double arg, Params... rest)
{
    std::printf(", %#.14Lgl", arg);
    PrintParams(rest...);
}
template<typename... Params>
static void PrintParams(double arg, Params... rest)
{
    std::printf(", %#.14g", arg);
    PrintParams(rest...);
}
template<typename... Params>
static void PrintParams(const void* arg, Params... rest)
{
    if(arg) std::printf(", (const void*)0x%llX", (unsigned long long)arg);
    else    std::printf(", nullptr");
    PrintParams(rest...);
}
template<typename... Params>
static void PrintParams(const char* arg, Params... rest)
{
    if(arg) std::printf(", \"%s\"", arg);
    else    std::printf(", nullptr");
    PrintParams(rest...);
}

static unsigned tests_failed = 0, tests_run = 0;

template<typename... Params>
void RunTest(const std::string& formatstr, Params... params)
{
    char result1[1024]{};
    char result2[1024]{};
    int out1 = __wrap_sprintf(result1, formatstr.c_str(), params...);
    int out2 = std::sprintf(  result2, formatstr.c_str(), params...);
    if(out1 != out2 || std::strcmp(result1, result2))
    {
        #pragma omp critical
        {
        std::printf("printf(\"%s\"", formatstr.c_str());
        PrintParams(params...);
        std::printf(");\n");
        std::printf("- tiny: %d [%s]\n", out1, result1);
        std::printf("- std:  %d [%s]\n", out2, result2);
        ++tests_failed;
        }
        #pragma omp atomic
        ++tests_run;
        return;
    }

    #pragma omp atomic
    ++tests_run;

#ifdef SUPPORT_SNPRINTF
    result1[0]='X'; result1[1]='\0';
    result2[0]='X'; result2[1]='\0';
    int limit = out2/2;
    int out3 = __wrap_snprintf(result1, limit, formatstr.c_str(), params...);
    int out4 = std::snprintf(  result2, limit, formatstr.c_str(), params...);
    if(out3 != out4 || std::strcmp(result1, result2))
    {
        #pragma omp critical
        {
        std::printf("snrintf(..., %d, \"%s\"", limit, formatstr.c_str());
        PrintParams(params...);
        std::printf(");\n");
        std::printf("- tiny: %d [%s]\n", out3, result1);
        std::printf("- std:  %d [%s]\n", out4, result2);
        ++tests_failed;
        }
    }
    #pragma omp atomic
    ++tests_run;
#endif
}
extern "C" {
int _write(int,const unsigned char*,unsigned,unsigned) { return 0; }
}

static void TortureTest()
{
}

int main()
{
    std::printf("Running regular tests...\n");

    RunTest("%%");
    RunTest("%d%d", 1234,5678);
    RunTest("%d%s", 1234,"test");
    RunTest("%s%d", "test",1234);
    RunTest("%s%s", "test","more");
    RunTest("%.2s%.2s", "test","more");
    RunTest("%4.02d", 3);
    RunTest("%4.02s", "test");
    { char a = 'A', b = 'B', c = 'C';
    RunTest("%c%c%c", a,b,c);
    RunTest("%d%d%d", a,b,c);
    }

    RunTest("%d%%%d", 1234,5678);
    RunTest("%d%%%s", 1234,"test");
    RunTest("%s%%%d", "test",1234);
    RunTest("%s%%%s", "test","more");
    RunTest("%.2s%%%.2s", "test","more");
    RunTest("a%4.02dc", 3);
    RunTest("d%4.02sf", "test");
    #pragma omp parallel for collapse(2)
    for(int wid1mode = 0; wid1mode <= 2; ++wid1mode)
    for(int wid2mode = 0; wid2mode <= 2; ++wid2mode)
    for(auto flag: flags)
    {
        for(int wid1 = -22; wid1 <= 22; ++wid1)
        for(int wid2 = -22; wid2 <= 22; ++wid2)
        {
            if(!wid1mode && wid1) continue;
            if(!wid2mode && wid2) continue;
            if(wid1mode==1 && wid1<0) continue;
            if(wid2mode==1 && wid2<0) continue;

            auto test = [&](const char* format, auto param)
            {
                const char format_char = std::strchr(format, '\0')[-1];
                const bool zero_pad    = std::strchr(flag, '0');
                if(zero_pad && (format_char=='s' || format_char=='c'))
                {
                    // Skip undefined test (zeropad on strings and chars)
                    return;
                }
                /*if(wid2 == 0 && format_char == 'c')
                {
                    // This differs from libc.
                    return;
                }*/
                if(wid2mode && !param && (format_char == 's' || format_char == 'p'))
                {
                    // This differs from libc (possibly undefined)
                    // E.g. printf("%.3s", "")
                    //        Libc prints "(null)", we print "(nu"
                    return;
                }
                if((format_char=='e' || format_char=='f' || format_char=='g' || format_char=='a'
                 || format_char=='E' || format_char=='F' || format_char=='G' || format_char=='A') && wid2>=16)
                {
                    return;
                }

                std::string start("%");
                std::string w1p = wid1mode ?     std::to_string(wid1) : std::string{};
                std::string w2p = wid2mode ? "."+std::to_string(wid2) : std::string{};
                if(wid1mode == 2 && wid2mode == 2)
                {
                    RunTest(start + flag + "*.*" + format, wid1, wid2, param);
                    if(SUPPORT_POSITIONAL_PARAMETERS)
                    {
                        RunTest(start + "3$" + flag + "*1$.*2$" + format, wid1, wid2, param);
                        RunTest(start + "3$" + flag + "*2$.*1$" + format, wid2, wid1, param);
                        RunTest(start + "2$" + flag + "*1$.*3$" + format, wid1, param, wid2);
                        if(wid1 == wid2)
                        {
                            RunTest(start + "1$" + flag + "*2$.*2$" + format, param, wid1);
                            RunTest(start + "2$" + flag + "*1$.*1$" + format, wid1, param);
                        }
                        #if __cplusplus >= 201700
                        if constexpr(sizeof(param) == sizeof(int))
                        {
                            if((int)param == wid1)
                            {
                                RunTest(start + "1$" + flag + "*1$.*2$" + format, param, wid2);
                            }
                            if((int)param == wid2)
                            {
                                RunTest(start + "2$" + flag + "*1$.*2$" + format, wid1, param);
                            }
                        }
                        #endif
                    }
                }
                else if(wid1mode == 2)
                {
                    RunTest(start + flag + "*" + w2p + format, wid1, param);
                    if(SUPPORT_POSITIONAL_PARAMETERS)
                    {
                        RunTest(start + "2$" + flag + "*1$" + w2p + format, wid1, param);
                        RunTest(start + "1$" + flag + "*2$" + w2p + format, param, wid1);
                    }
                }
                else if(wid2mode == 2)
                {
                    RunTest(start + flag + w1p + ".*" + format, wid2, param);
                    if(SUPPORT_POSITIONAL_PARAMETERS)
                    {
                        RunTest(start + "2$" + flag + w1p + ".*1$" + format, wid2, param);
                        RunTest(start + "1$" + flag + w1p + ".*2$" + format, param, wid2);
                    }
                }
                else
                {
                    RunTest(start + flag + w1p + w2p + format, param);
                    if(SUPPORT_POSITIONAL_PARAMETERS)
                    {
                        RunTest(start + "1$" + flag + w1p + w2p + format, param);
                        RunTest(start + "01$" + flag + w1p + w2p + format, param);
                    }
                }
            };
            test("s", (const char*)nullptr);
            test("s", (const char*)"");
            test("s", (const char*)"quix");
            test("s", (const char*)"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
            test("c", int('\0'));
            test("c", int('A'));
            test("c", int(228-256));
            test("p", (const void*)nullptr);
            test("p", (const void*)0x12345678);
            test("p", (const void*)0xE2345678);
            test("p", (const void*)0x1234567812345678ll);
            test("p", (const void*)0xE234567812345678ll);
            test("d", int(0));
            test("d", int(3));
            test("d", int(-600000));
            test("d", int(600000));
            test("u", int(0));
            test("u", int(-600000));
            test("u", int(600000));
            test("ld", long(0));
            test("ld", long(-600000));
            test("ld", long(600000));
            test("lu", long(0));
            test("lu", long(-600000));
            test("lu", long(600000));
            test("lld", (long long)(0));
            test("lld", (long long)(-60000000000000ll));
            test("lld", (long long)(60000000000000ll));
            test("llu", (long long)(0));
            test("llu", (long long)(-60000000000000ll));
            test("llu", (long long)(60000000000000ll));
            test("x", 600000);
            test("X", 600000);
            test("o", 600000);
            test("x", -600000);
            test("X", -600000);
            test("o", -600000);
            test("x", 0);
            test("X", 0);
            test("o", 0);
            test("d", int(0x80000000u));
            test("i", int(0x80000000u));
            test("u", int(0x80000000u));
            test("x", int(0x80000000u));
            test("X", int(0x80000000u));
            test("o", int(0x80000000u));
            test("d", int(0xFFFFFFFFu));
            test("i", int(0xFFFFFFFFu));
            test("u", int(0xFFFFFFFFu));
            test("x", int(0xFFFFFFFFu));
            test("X", int(0xFFFFFFFFu));
            test("o", int(0xFFFFFFFFu));
            test("d", int(0x7FFFFFFFu));
            test("i", int(0x7FFFFFFFu));
            test("u", int(0x7FFFFFFFu));
            test("x", int(0x7FFFFFFFu));
            test("X", int(0x7FFFFFFFu));
            test("o", int(0x7FFFFFFFu));
            test("lld", (long long)(0x8000000000000000ull));
            test("lli", (long long)(0x8000000000000000ull));
            test("llu", (long long)(0x8000000000000000ull));
            test("llx", (long long)(0x8000000000000000ull));
            test("llX", (long long)(0x8000000000000000ull));
            test("llo", (long long)(0x8000000000000000ull));
            test("lld", (long long)(0xFFFFFFFFFFFFFFFFull));
            test("lli", (long long)(0xFFFFFFFFFFFFFFFFull));
            test("llu", (long long)(0xFFFFFFFFFFFFFFFFull));
            test("llx", (long long)(0xFFFFFFFFFFFFFFFFull));
            test("llX", (long long)(0xFFFFFFFFFFFFFFFFull));
            test("llo", (long long)(0xFFFFFFFFFFFFFFFFull));
            test("lld", (long long)(0x7FFFFFFFFFFFFFFFull));
            test("lli", (long long)(0x7FFFFFFFFFFFFFFFull));
            test("llu", (long long)(0x7FFFFFFFFFFFFFFFull));
            test("llx", (long long)(0x7FFFFFFFFFFFFFFFull));
            test("llX", (long long)(0x7FFFFFFFFFFFFFFFull));
            test("llo", (long long)(0x7FFFFFFFFFFFFFFFull));
            if(SUPPORT_H_LENGTHS)
            {
                test("hd", int(0));
                test("hd", int(20000));
                test("hd", int(-20000));
                test("hd", int(-600000));
                test("hd", int(600000));
                test("hu", int(0));
                test("hu", int(20000));
                test("hu", int(-20000));
                test("hu", int(-600000));
                test("hu", int(600000));
                test("hhd", int(0));
                test("hhd", int(100));
                test("hhd", int(-100));
                test("hhd", int(-600000));
                test("hhd", int(600000));
                test("hhu", int(0));
                test("hhu", int(100));
                test("hhu", int(-100));
                test("hhu", int(-600000));
                test("hhu", int(600000));
            }
            if(SUPPORT_FLOAT_FORMATS)
            {
                test("e", 10000.);
                test("e", 0.000123456789);
                test("e", 1.23456789);
                test("e", 123456.789);
                test("e", 1.5e42);
                test("e", 1.5e-42);
                test("e", 9.9999);
                test("e", 0.);
                test("e", 1./0.);
                test("f", 10000.);
                test("f", 0.000123456789);
                test("f", 1.23456789);
                test("f", 123456.789);
                test("f", 1.5e6);
                test("f", 1.5e-6);
                test("f", 9.9999);
                test("f", 0.);
                test("f", 1./0.);
            }
        }
    }

    std::printf("Running torture test...\n");
    TortureTest();

    if(tests_failed)
        std::printf("%u tests run, %u tests failed\n", tests_run, tests_failed);
    else
        std::printf("%u tests run, all OK\n", tests_run);
}
