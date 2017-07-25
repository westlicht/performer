#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <utility>
#include <memory>
#include <cmath>

// #define SUPPORT_SNPRINTF
//#define SUPPORT_ASPRINTF
//#define SUPPORT_FIPRINTF
// #define SUPPORT_FILE_FUNCTIONS

static constexpr bool SUPPORT_BINARY_FORMAT = false;// Whether to support %b format type
static constexpr bool STRICT_COMPLIANCE     = true;
static constexpr bool SUPPORT_N_FORMAT      = true; // Whether to support %n format type
static constexpr bool SUPPORT_H_LENGTHS     = true; // Whether to support h and hh length modifiers
static constexpr bool SUPPORT_T_LENGTH      = true; // Whether to support t length modifier
static constexpr bool SUPPORT_J_LENGTH      = true; // Whether to support j length modifier
static constexpr bool SUPPORT_FLOAT_FORMATS = true; // Floating pointing formats
static constexpr bool SUPPORT_A_FORMAT      = false; // Floating point hex format
static constexpr bool SUPPORT_LONG_DOUBLE   = false;
static constexpr bool SUPPORT_POSITIONAL_PARAMETERS = false;

#ifdef __GNUC__
 #define NOINLINE   __attribute__((noinline))
 #define USED_FUNC  __attribute__((used,noinline))
 #define VERYINLINE __attribute__((optimize("inline-functions"),always_inline))
 #pragma GCC push_options
 #pragma GCC optimize ("Os")
 /**/
 #pragma GCC optimize ("no-align-functions")
 #pragma GCC optimize ("no-align-jumps")
 #pragma GCC optimize ("no-align-loops")
 #pragma GCC optimize ("no-align-labels")
 #pragma GCC optimize ("reorder-blocks")
 // #pragma GCC optimize ("reorder-blocks-and-partition")
 #pragma GCC optimize ("prefetch-loop-arrays")
 /**/
 //#pragma GCC optimize ("no-ipa-cp-clone")
 #pragma GCC optimize ("inline-functions")
 //#pragma GCC optimize ("conserve-stack")
 //#pragma GCC optimize ("no-defer-pop")
 #pragma GCC optimize ("tree-switch-conversion")
 //#pragma GCC optimize ("param=case-values-threshold=1")
 #define likely(x)   __builtin_expect(!!(x), 1)
 #define unlikely(x) __builtin_expect(!!(x), 0)
#else
 #define NOINLINE
 #define USED_FUNC
 #define likely(x)   (x)
 #define unlikely(x) (x)
#endif
#if __cplusplus >= 201400 && (!defined(__GNUC__) || __GNUC__ >= 7)
 #define PASSTHRU   [[fallthrough]];
#else
 #define PASSTHRU
#endif
#if __cplusplus >= 201700 && (!defined(__GNUC__) || __GNUC__ >= 7)
 #define if_constexpr if constexpr
#else
 #define if_constexpr if
#endif
namespace
{
namespace myprintf
{
    typedef std::int_fast64_t  intfmt_t;
    typedef std::uint_fast64_t uintfmt_t;

    static_assert(sizeof(intfmt_t) >= sizeof(long long), "We are unable to print longlong types");
    static_assert(sizeof(intfmt_t) >= sizeof(std::intmax_t), "We are unable to print intmax_t types");
    static_assert(sizeof(std::ptrdiff_t) == sizeof(long long)
               || sizeof(std::ptrdiff_t) == sizeof(long), "We may have problems with %td format");
    static_assert(sizeof(std::size_t) == sizeof(long long)
               || sizeof(std::size_t) == sizeof(long), "We may have problems with %zd format");
    static_assert(sizeof(std::intmax_t) == sizeof(long long)
               || sizeof(std::intmax_t) == sizeof(long), "We may have problems with %jd format");

    // base is one of these:
    static constexpr unsigned char base_decimal   = 10;
    static constexpr unsigned char base_hex       = 16;
    static constexpr unsigned char base_octal     = 8;
    static constexpr unsigned char base_binary    = 2;

    // state.fmt_flags is a bitmask of these:
    // - Flags 0x1F are common to all formats.
    // - Flags 0xFE are interpreted in format_integer().
    // - Flags 0x03 are interpreted in format_string().
    // - Flags 0xFE are interpreted in format_float(), but with different meanings.
    static constexpr unsigned char fmt_leftalign = 0x01; // '-'
    static constexpr unsigned char fmt_zeropad   = 0x02; // '0'
    static constexpr unsigned char fmt_plussign  = 0x04; // '+'
    static constexpr unsigned char fmt_space     = 0x08; // ' '
    static constexpr unsigned char fmt_alt       = 0x10; // '#'
    static constexpr unsigned char fmt_ucbase    = 0x20; // capital hex (%X)
    static constexpr unsigned char fmt_pointer   = 0x40; // p format
    static constexpr unsigned char fmt_signed    = 0x80; // d,i,p formats
    static constexpr unsigned char fmt_exponent  = 0x40;
    static constexpr unsigned char fmt_autofloat = 0x80;

    static constexpr unsigned PatternLength = 8; // number of spaces/zeros in stringconstants

    template<bool SupportFloats> struct GetStringConstants{};
    template<> struct GetStringConstants<false>
    {
        static const char* GetTable() VERYINLINE
        {
            static const char stringconstants_intonly[] {
                // eight spaces
                ' ',' ',' ',' ', ' ',' ',' ',' ',
                // eight zeros
                '0','0','0','0', '0','0','0','0',
                // table of some multichar prefixes (15 letters)
                /*'0',*/'x',  '0', 'X',
                '(','n','i','l',')', '(','n','u','l','l',')',
                // single-char prefixes
                '-','+',' ',
                // multichar-prefix specs
                (0),      (2*32+0), (2*32+2), // "", 0x, 0X
                char(5*32+4), char(6*32+9),   // nil,null
            };
            return stringconstants_intonly;
        }
    };
    template<> struct GetStringConstants<true>
    {
        static const char* GetTable() VERYINLINE
        {
            static const char stringconstants_floats[] {
                // eight spaces
                ' ',' ',' ',' ', ' ',' ',' ',' ',
                // eight zeros
                '0','0','0','0', '0','0','0','0',
                // table of some multichar prefixes (27 letters)
                /*'0',*/'x','n','a','n','i','n','f',
                '0',    'X','N','A','N','I','N','F',
                '(','n','i','l',')', '(','n','u','l','l',')',
                // single-char prefixes
                '-','+',' ',
                // multichar-prefix specs
                (0),      (2*32+0), (2*32+8),            // "", 0x, 0X
                char(5*32+16), char(6*32+21),            // nil,null
                (3*32+2), (3*32+5), (3*32+10), (3*32+13) // nan,inf,NAN,INF
            };
            return stringconstants_floats;
        }
    };
    static constexpr unsigned char prefix_minus = 1;
    static constexpr unsigned char prefix_plus  = 2;
    static constexpr unsigned char prefix_space = 3;
    static constexpr unsigned char prefix_0x    = 4*1;
    static constexpr unsigned char prefix_0X    = 4*2;
    static constexpr unsigned char prefix_nil   = 4*3;
    static constexpr unsigned char prefix_null  = 4*4;
    static constexpr unsigned char prefix_nan   = 4*5;
    static constexpr unsigned char prefix_inf   = 4*6;
    static constexpr unsigned char prefix_NAN   = 4*7;
    static constexpr unsigned char prefix_INF   = 4*8;
    static constexpr unsigned char prefix_data_length = SUPPORT_FLOAT_FORMATS ? (8+8+5+6) : (2+2+5+6);

    static constexpr unsigned NUMBUFFER_SIZE = SUPPORT_BINARY_FORMAT ? 64 : 23;

    #define BASE_MUL 0x8u
    #define FLAG_MUL 0x10000u
    #define PFX_MUL  0x200u

    #define set_sizebase(base,type) \
        (fmt_flags = (fmt_flags % FLAG_MUL) + FLAG_MUL * ((base/2-1) + BASE_MUL * (sizeof(type)-1)))
    #define set_size(type) \
        (fmt_flags = (fmt_flags % (FLAG_MUL * BASE_MUL)) + FLAG_MUL*BASE_MUL * (sizeof(type)-1))
    #define set_base(base) \
        (fmt_flags = (fmt_flags & ((FLAG_MUL-1) + unsigned(~0ull * FLAG_MUL*BASE_MUL))) + (FLAG_MUL*(base/2-1)))
    #define get_base() \
        ((fmt_flags / FLAG_MUL) % BASE_MUL + 1)*2
    #define get_type() \
        (fmt_flags / (FLAG_MUL * BASE_MUL)+1)
    #define is_type(type) \
        (get_type() == sizeof(type))

    inline unsigned clamp(unsigned value, unsigned minvalue, unsigned maxvalue) VERYINLINE;
    inline unsigned clamp(unsigned value, unsigned minvalue, unsigned maxvalue)
    {
        if(value < minvalue) value = minvalue;
        if(value > maxvalue) value = maxvalue;
        return value;
    }
    inline unsigned estimate_uinteger_width(uintfmt_t uvalue, unsigned base) VERYINLINE /*NOINLINE*/;
    inline unsigned estimate_uinteger_width(uintfmt_t uvalue, unsigned base)
    {
        unsigned width = 0;
        while(uvalue != 0)
        {
            ++width;
            uvalue /= base;
        }
        return width;
    }

    void put_uinteger(char* target, uintfmt_t uvalue, unsigned width, unsigned  base, int alphaoffset) /*NOINLINE*/
    {
        for(unsigned w=width; w-- > 0; )
        {
            // FIXME: gcc-arm-embedded calls __aeabi_uldivmod twice here for no reason
            unsigned digitvalue = uvalue % base; uvalue /= base;
            target[w] = digitvalue + (likely(digitvalue < 10) ? '0' : alphaoffset);
        }
    }
    void put_uint_decimal(char* target, uintfmt_t uvalue, unsigned width) NOINLINE;
    void put_uint_decimal(char* target, uintfmt_t uvalue, unsigned width)
    {
        put_uinteger(target, uvalue, width, 10, '0');
    }

    inline std::pair<unsigned,unsigned> format_integer
        (char* numbuffer, intfmt_t value, unsigned fmt_flags, unsigned min_digits) VERYINLINE;
    inline std::pair<unsigned,unsigned> format_integer
        (char* numbuffer, intfmt_t value, unsigned fmt_flags, unsigned min_digits)
    {
        // Maximum length is ceil(log8(2^64)) = ceil(64/3+1) = 23 characters (+1 for octal leading zero)
        static_assert(NUMBUFFER_SIZE >= (SUPPORT_BINARY_FORMAT ? 64 : 23), "Too small numbuffer");

        if(fmt_flags & fmt_pointer)
        {
            if(unlikely(!value)) { return {0u, fmt_flags + PFX_MUL*prefix_nil}; } // (nil) and %p, no other prefix
            if_constexpr(STRICT_COMPLIANCE) goto signed_flags;
        }
        if(fmt_flags & fmt_signed)
        {
            if(value < 0)                   { value = -value; fmt_flags += PFX_MUL*prefix_minus; }
            else signed_flags: fmt_flags += ((((prefix_plus*((1u << fmt_plussign)
                                                           + (1u << (fmt_plussign+fmt_space)))
                                             + prefix_space*((1u << fmt_space)))*PFX_MUL) >> (fmt_flags & (fmt_plussign|fmt_space)))
                                               & (PFX_MUL*(prefix_plus|prefix_space)));
            /*else signed_flags: if(fmt_flags & fmt_plussign) { fmt_flags += PFX_MUL*prefix_plus;  } // 0,4,8,12 -> 0,2,3,2
            else               if(fmt_flags & fmt_space)    { fmt_flags += PFX_MUL*prefix_space; }*/
                            //fmt_flags += (fmt_flags&fmt_space) * (PFX_MUL*prefix_space) / fmt_space;

            // GNU libc printf ignores '+' and ' ' modifiers on unsigned formats, but curiously, not for %p.
            // Note that '+' overrides ' ' if both are used.
        }

        unsigned b = get_base();
        unsigned width = estimate_uinteger_width(value, b);
        if(STRICT_COMPLIANCE && unlikely(fmt_flags & (fmt_alt | fmt_pointer)))
        {
            // Bases: 2   /2 = 1  -1 = 0
            //        8   /2 = 4  -1 = 3
            //        10  /2 = 5  -1 = 4
            //        16  /2 = 8  -1 = 7
            /*switch(b)
            {
                case 8:
                    // Make sure there's at least 1 leading '0'
                    // Note: if value=0, width=0
                    ++width;
                    break;
                case 16:
                    // Add 0x/0X prefix
                    //if(value != 0) { fmt_flags += PFX_MUL*((fmt_flags & fmt_ucbase) ? prefix_0X : prefix_0x); }
                    if(width>0) fmt_flags += (PFX_MUL*prefix_0x + (fmt_flags&fmt_ucbase)*(PFX_MUL*prefix_0X-PFX_MUL*prefix_0x)/fmt_ucbase);
                    break;
            }*/
            /*switch((b/2)-4)
            {
                case 10/2-4: break;
                case 8/2-4: ++width; break;
                case 16/2-4: if(width>0) fmt_flags += (PFX_MUL*prefix_0x + (fmt_flags&fmt_ucbase)*(PFX_MUL*prefix_0X-PFX_MUL*prefix_0x)/fmt_ucbase); break;
                default: if_constexpr(!SUPPORT_BINARY_FORMAT) __builtin_unreachable(); break;
            }*/
            if(!(b&7)) { // bases 8 or 16 only
                if(b&8) // base 8
                    ++width;
                else // base 16
                    if(width>0) fmt_flags += (PFX_MUL*prefix_0x + (fmt_flags&fmt_ucbase)*(PFX_MUL*prefix_0X-PFX_MUL*prefix_0x)/fmt_ucbase);
            }
        }

        // Range check
        width = clamp(width, min_digits, NUMBUFFER_SIZE);
        //put_uinteger(numbuffer, value, width, b, ((fmt_flags & fmt_ucbase) ? 'A' : 'a')-10);
        put_uinteger(numbuffer, value, width, b, ('a'-10  -  (('a'-'A')*((fmt_flags & fmt_ucbase)/fmt_ucbase))));
        return {width,fmt_flags};
    }

    template<typename FloatType>
    inline std::pair<unsigned,unsigned> format_float
        (char* numbuffer, FloatType value, unsigned fmt_flags, unsigned precision)
    {
        unsigned char prefix_index = 0;
        if(value < 0)     { value = -value; prefix_index = prefix_minus; }
        else if(fmt_flags & fmt_plussign) { prefix_index = prefix_plus;  }
        else if(fmt_flags & fmt_space)    { prefix_index = prefix_space; }

        if(!std::isfinite(value))
        {
            return {0u, fmt_flags + PFX_MUL*(prefix_index + (
                                std::isinf(value) ? ((fmt_flags & fmt_ucbase) ? prefix_INF : prefix_inf)
                                                  : ((fmt_flags & fmt_ucbase) ? prefix_NAN : prefix_nan))) };
        }

        int e_exponent=0;

        if(SUPPORT_A_FORMAT && get_base() == base_hex)
        {
            value = std::frexp(value, &e_exponent);
            while(value > 0 && value*2 < 0x10) { value *= 2; --e_exponent; }
        }
        else if(value != FloatType(0))
        {
            e_exponent = std::floor(std::log10(value));
        }

        if(precision == ~0u) precision = 6;
        if(fmt_flags & fmt_autofloat)
        {
            // Mode: Let X = E-style exponent, P = chosen precision.
            //       If P > X >= -4, choose 'f' and P = P-1-X.
            //       Else,           choose 'e' and P = P-1.
            if(!precision) precision = 1;
            if(int(precision) > e_exponent && e_exponent >= -4) { precision -= e_exponent+1; }
            else                                                { precision -= 1; fmt_flags |= fmt_exponent; }
        }

        int head_width = 1;

        /* Round the value into the specified precision */
        uintfmt_t uvalue = 0;
        if(value != FloatType(0))
        {
            if(!(fmt_flags & fmt_exponent))
            {
                head_width = 1 + e_exponent;
            }
            unsigned total_precision = precision;
            /*if(head_width > 0)*/ total_precision += head_width;

            // Create a scaling factor where all desired decimals are in the integer portion
            FloatType factor = std::pow(FloatType(10), FloatType(total_precision - std::ceil(std::log10(value))));
            //auto ovalue = value;
            auto rvalue = std::round(value * factor);
            uvalue = rvalue;
            // Scale it back
            value = rvalue / factor;
            // Recalculate exponent from rounded value
            e_exponent = std::floor(std::log10(value));
            if(!(fmt_flags & fmt_exponent))
            {
                head_width = 1 + e_exponent;
            }

            /*std::printf("Value %.12g rounded to %u decimals: %.12g, %lu  head_width=%d\n",
                ovalue, total_precision, value, uvalue, head_width);*/
        }

        unsigned exponent_width = 0;
        unsigned point_width    = (precision > 0 || (fmt_flags & fmt_alt)) ? 1 : 0;
        unsigned decimals_width = clamp(precision, 0, sizeof(numbuffer)-exponent_width-point_width-head_width);

        // Count the number of digits
        uintfmt_t digits = estimate_uinteger_width(uvalue, 10);
        uintfmt_t scale    = std::pow(FloatType(10), FloatType(int(digits - head_width)));
        if(!scale) scale=1;
        uintfmt_t head     = uvalue / scale;
        uintfmt_t fraction = uvalue % scale;
        //uintfmt_t scale2   = uintfmt_t(std::pow(FloatType(10), FloatType(-head_width)));
        //if(head_width < 0 && scale2 != 0) fraction /= scale2;

        /*std::printf("- digits in %lu=%lu, scale=%lu, head=%lu, fraction=%lu\n",
            uvalue,digits, scale, head,fraction);*/

        if(head_width < 1) head_width = 1;

        if(fmt_flags & fmt_exponent)
        {
        /*
            auto vv = value * std::pow(FloatType(10), FloatType(-e_exponent));

            fraction = std::modf(vv, &head);
            //if(value != FloatType(0) && head == FloatType(0)) { vv *= 10; fraction = std::modf(vv, &head); --e_exponent; }

            auto uf = fraction;
            fraction = std::round(fraction * std::pow(FloatType(10), FloatType(int(precision))));
            std::printf("%.20g -> %.20g split into head=%.20g, fraction=%.20g -> %.20g using exponents %d\n",
                value,vv,head,uf,fraction,
                e_exponent);
        */
            exponent_width =
                clamp(estimate_uinteger_width(e_exponent<0 ? -e_exponent : e_exponent, 10),
                      2,
                      sizeof(numbuffer)-3-head_width) + 2;
        }
        else
        {
        }

        put_uint_decimal(numbuffer + 0, head, head_width);
        unsigned tgt = head_width;
        if(point_width)
        {
            numbuffer[tgt] = '.'; tgt += 1;
            put_uint_decimal(numbuffer + tgt, fraction, decimals_width);
            tgt += decimals_width;
        }
        if(exponent_width)
        {
            numbuffer[tgt++] = ((fmt_flags & fmt_ucbase) ? 'E' : 'e');
            numbuffer[tgt++] = ((e_exponent < 0)         ? '-' : '+');
            put_uint_decimal(numbuffer + tgt, e_exponent<0 ? -e_exponent : e_exponent, exponent_width-2);
            tgt += exponent_width-2;
        }
        return {tgt, fmt_flags + PFX_MUL*prefix_index};
    }

    struct prn
    {
        char* param;
        void (*put)(char*,const char*,std::size_t);

        const char* putbegin = nullptr;
        const char* putend   = nullptr;

        char prefixbuffer[SUPPORT_FLOAT_FORMATS ? 4 : 3]; // Longest: +inf or +0x

        void flush() NOINLINE
        {
            if(likely(putend != putbegin))
            {
                unsigned n = putend-putbegin;
                //std::printf("Flushes %d from <%.*s> to %p\n", n,n,putbegin, param);
                const char* start = putbegin;
                char*      pparam = param;
                // Make sure that the same content will not be printed twice
                //putbegin = start  + n;
                param    = pparam + n;
                put(pparam, start, n);
                //std::printf("As a result, %p has <%.*s>\n", param, n, param);
            }
        }
        void append(const char* source, unsigned length) NOINLINE
        {
            //std::printf("Append %d from <%.*s>\n", length, length, source);
            //if(likely(length != 0))
            {
                if(source != putend)
                {
                    flush();
                    putbegin = source;
                }
                putend = source+length;
            }
        }
        void append_spaces(const char* from, unsigned count) VERYINLINE
        {
            while(count > 0)
            {
                unsigned n = std::min(count, PatternLength);
                append(from, n);
                count -= n;
            }
        }

        inline void format_string(const char* source, unsigned sourcelength,
                                  unsigned min_width, unsigned max_width, unsigned fmt_flags) VERYINLINE
        {
            unsigned char prefix_index = (fmt_flags / PFX_MUL) % (FLAG_MUL/PFX_MUL);

            // Don't zeropad when there are textual prefixes, or if leftaligning is set
            if(STRICT_COMPLIANCE && (unlikely(prefix_index >= prefix_nil) || (fmt_flags & fmt_leftalign)))
            {
                fmt_flags &= ~fmt_zeropad;
            }

            const char* stringconstants = GetStringConstants<SUPPORT_FLOAT_FORMATS>::GetTable();
            unsigned char ctrl = stringconstants[PatternLength*2 + prefix_data_length-1+3 + prefix_index/4];
            unsigned prefixlength = ctrl/32;
            const char* prefixsource = &stringconstants[PatternLength*2-1 + (ctrl%32)];
            const char* prefix = prefixsource;
            if(prefix_index & 3)
            {
                prefix = prefixbuffer;
                prefixbuffer[0] = stringconstants[(prefix_index&3) + PatternLength*2 + prefix_data_length-1 -1];
                std::memcpy(&prefixbuffer[1], prefixsource, prefixlength++);
            }

            //stringconstants += (fmt_flags & fmt_zeropad)?PatternLength:0;
            //stringconstants = (fmt_flags&fmt_zeropad)? GetStringConstants<SUPPORT_FLOAT_FORMATS>::GetTable()+PatternLength
            //                                         : GetStringConstants<SUPPORT_FLOAT_FORMATS>::GetTable();
            stringconstants += PatternLength*(fmt_flags & fmt_zeropad)/fmt_zeropad;

            // Calculate length of prefix + source
            unsigned combined_length = sourcelength + prefixlength;
            // Clamp it into maximum permitted width
            if(combined_length > max_width)
            {
                combined_length = max_width;
                // Figure out how to divide this between prefix and source
                // By default, shorten the source, but print full prefix
                sourcelength = combined_length - prefixlength;
                // Only room to print some of the prefix, and nothing of the source?
                if(unlikely(combined_length < prefixlength))
                {
                    prefixlength = combined_length;
                    sourcelength = 0;
                }
            }
            // Calculate the padding width
            unsigned padding_width = min_width > combined_length ? min_width - combined_length : 0;

            /* There are three possible combinations:
             *
             *    Leftalign      prefix, source, spaces
             *    Zeropad        prefix, zeros,  source
             *    neither        spaces, prefix, source
             *
             * Note that in case of zeropad+leftalign,
             * zeropad is disregarded according to the standard.
             */

            // other(0):     1,2,0 = 1*1+2*4+0*16 = 0x09 (6 bits)
            // leftalign(1): 2,0,1 = 2*1+0*4+1*16 = 0x12 (6 bits)
            // zeropad(2):   2,1,0 = 2*1+1*4+0*16 = 0x06 (6 bits)
            // (invalid)(3): 0,1,2
            unsigned m = (1*1 + 2*4 + 0*16) * (1)
                       + (2*1 + 0*4 + 1*16) * ((1u << (8*fmt_leftalign)) + (1u << (8*(fmt_leftalign+fmt_zeropad))))
                       + (2*1 + 1*4 + 0*16) * ((1u << (8*fmt_zeropad)));
            m >>= ((fmt_flags&(fmt_leftalign+fmt_zeropad))*8);
            for(unsigned r=0; r<3; ++r, m>>=2)
            {
                if(m&1)      append_spaces(stringconstants,padding_width);
                else if(m&2) append(prefix, prefixlength);
                else         append(source, sourcelength);
            }
/*
            if( (fmt_flags & (fmt_leftalign | fmt_zeropad))) append(prefix, prefixlength);
            if( (fmt_flags & fmt_leftalign))                 append(source, sourcelength);
            append_spaces(stringconstants, padding_width);
            if(!(fmt_flags & (fmt_leftalign | fmt_zeropad))) append(prefix, prefixlength);
            if(!(fmt_flags & fmt_leftalign))                 append(source, sourcelength);*/
        }
    };

    unsigned read_int(const char*& fmt, unsigned def)
    {
        if(*fmt >= '0' && *fmt <= '9')
        {
            unsigned v = 0;
            do { v = v*10 + (*fmt++ - '0'); } while(*fmt >= '0' && *fmt <= '9');
            return v;
        }
        return def;
    }

    /*inline unsigned read_param_index(const char*& fmt) VERYINLINE;
    inline unsigned read_param_index(const char*& fmt)
    {
        const char* bkup = fmt;
        unsigned index = read_int(fmt, 0);
        if(*fmt == '$') { ++fmt; return index; }
        fmt = bkup;
        return 0;
    }*/

    template<bool DoOperation> struct auto_dealloc_pointer {};
    template<> struct auto_dealloc_pointer<false> { typedef unsigned char* type; };
    template<> struct auto_dealloc_pointer<true>  { typedef std::unique_ptr<unsigned char[]> type; };

    template<unsigned tagv,typename T> struct typetag { typedef T type; static constexpr unsigned tag = tagv; };

    /* Note: Compilation of this function depends on the compiler's ability to optimize away
     * code that is never reached because of the state of the constexpr bools.
     * E.g. if SUPPORT_POSITIONAL_PARAMETERS = false, much of the code in this function
     * will end up dummied out and the binary size will be smaller.
     */
    int myvprintf(const char* fmt, std::va_list ap, char* param, void (*put)(char*,const char*,std::size_t)) NOINLINE;
    int myvprintf(const char* fmt_begin, std::va_list ap, char* param, void (*put)(char*,const char*,std::size_t))
    {
        prn state;
        state.param = param;
        state.put   = put;

        char numbuffer[NUMBUFFER_SIZE];

        /* Positional parameters support:
         * Pass 3: Calculate the number of parameters,
                   then allocate array of sizes
                   Or, if no positional parameters were found,
                   jump straight to step 0
         * Pass 2: Populate the array of sizes,
         *         then convert it into array of offsets,
         *         and allocate array of data,
         *         and populate array of data
         * Pass 1: Actually print,
         *         then free the two arrays, and exit
         * Pass 0: Actually print (no pos. params)
         */
        //printf("---Interpret %s\n", fmt_begin);

        constexpr unsigned MAX_AUTO_PARAMS = 0x10000, MAX_ROUNDS = 4, POS_PARAM_MUL = MAX_AUTO_PARAMS * MAX_ROUNDS;
        constexpr unsigned MAX_EXPLICIT_PARAMS = 0x400;
        auto_dealloc_pointer<SUPPORT_POSITIONAL_PARAMETERS>::type param_data_table{};

        // Figure out the largest parameter size. This is a compile-time constant.
        constexpr std::size_t largest = sizeof(long long);
        // std::max(std::max(sizeof(long long), sizeof(void*)),
        //                                          SUPPORT_FLOAT_FORMATS ? std::max(sizeof(double),
        //                                            SUPPORT_LONG_DOUBLE ? sizeof(long double) : sizeof(long))
        //                                                                : sizeof(long));

        // "Round" variable encodes, starting from lsb:
        //     - log2(MAX_AUTO_PARAMS) bits: number of auto params counted so far
        //     - 2 bits:                     round number
        //     - The rest:                   maximum explicit param index found so far
        for(unsigned round = SUPPORT_POSITIONAL_PARAMETERS ? (3*MAX_AUTO_PARAMS) : 0; ; )
        {
            auto process_param = [&round,table=&param_data_table[0]](unsigned typetag, unsigned which_param_index) -> void*
            {
                if(which_param_index == 0)
                {
                    // Automatic parameter index; increment counter, and use the possibly-wrapped-around counter
                    which_param_index = round++ % MAX_AUTO_PARAMS;
                }
                else
                {
                    // Explicit parameter index (1-n)
                    // Range check
                    which_param_index = (which_param_index-1) % MAX_EXPLICIT_PARAMS + 1;
                    // Update the max counter
                    if(which_param_index > round / POS_PARAM_MUL)
                    {
                        round = round % POS_PARAM_MUL + which_param_index * POS_PARAM_MUL;
                    }
                    // Decrease by 1 in order to use as an array index
                    --which_param_index;
                }
                unsigned short* param_offset_table = reinterpret_cast<unsigned short *>(&table[0]);
                switch((round / MAX_AUTO_PARAMS) % MAX_ROUNDS)
                {
                    // Round 3: Collect parameter counts to know how big array to allocate
                    // Round 2: Deposit parameter sizes in array that has been allocated
                    // Round 1: Return pointers to data that has been loaded
                    // Round 0: Direct loading, this function is not used
                    case 2: param_offset_table[which_param_index] = typetag; break;
                    case 1: return &table[largest * param_offset_table[which_param_index]]; break;
                    #ifdef __GNUC__
                    case 3: break; // Just collecting parameter counts
                    default: __builtin_unreachable();
                    #else
                    default: break;
                    #endif
                }
                return nullptr;
            };

            // Start parsing the format string from beginning
            for(const char* fmt = fmt_begin; likely(*fmt != '\0'); ++fmt)
            {
                if(likely(*fmt != '%'))
                {
                literal:;
                    // Rounds 0 and 1 are action rounds. Rounds 2 and 3 are not (nothing is printed).
                    if_constexpr(SUPPORT_POSITIONAL_PARAMETERS) { if(round & (MAX_AUTO_PARAMS*2)) continue; }
                    state.append(fmt, 1);
                    continue;
                }

                #define GET_ARG(acquire_type, variable, type_index, which_param_index, ifnot) \
                    acquire_type variable; \
                    if(SUPPORT_POSITIONAL_PARAMETERS && round != 0) \
                    { \
                        void* p = process_param(type_index, which_param_index); \
                        if(p) { variable = *(acquire_type const*)p; } \
                        else  { ifnot; } \
                    } \
                    else variable = va_arg(ap, acquire_type);

                // Read format flags
                constexpr unsigned got_minwidth  = 0x100u;
                unsigned min_width = 0, precision = ~0u, fmt_flags = 0;
                // fmt_flags:
                //    bits 0-7:   state.fmt_flags
                //    bit 8:      flag: min_width has been read
                //    bits 9-14:  prefix index (after numeric format)
                //    bits 16-18: numeric base/2-1
                //    bits 19-31: parameter size-1
                set_sizebase(base_decimal, int);

                // The numeric base is encoded into the same variable as fmt_flags
                // to reduce the number of local variables,
                // thereby reducing register pressure, stack usage,
                // spilling etc., resulting in a smaller compiled binary.

                const char* source = numbuffer;
                unsigned    length = 0;

                unsigned param_index = 0;
                /*if_constexpr(SUPPORT_POSITIONAL_PARAMETERS)
                {
                    // Read possible position-index for the value (it comes before flags / widths)
                    ++fmt;
                    param_index = read_param_index(fmt);
                    goto moreflags;
                }*/
            moreflags1:
                ++fmt;
            moreflags:;
                /* Note that this state machine is designed with the assumption of valid
                 * format string. If there are e.g. flag characters where they
                 * should not be, all sorts of things will go wrong here.
                 */

                /*
                    '\0'
                    - +#
                    0
                    123456789
                    $
                    .
                    *
                    tzlLjh
                    nsc
                    pxXobdiu
                    aAeEgGfF
                    % (((and other)))
                */
                switch(*fmt)
                {
                    case '\0': goto unexpected;
                    case '%': got_unk: default: goto literal;

                    case '-': fmt_flags |= fmt_leftalign; goto moreflags1;
                    case ' ': fmt_flags |= fmt_space;     goto moreflags1;
                    case '+': fmt_flags |= fmt_plussign;  goto moreflags1;
                    case '#': fmt_flags |= fmt_alt;       goto moreflags1;
                    case '0': if(likely(!(fmt_flags & got_minwidth)))
                              { fmt_flags |= fmt_zeropad; goto moreflags1; }
                              PASSTHRU
                    case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9':
                    {
                        // Read min-width or precision
                        unsigned value = read_int(fmt, 0);
                        if(!(fmt_flags & got_minwidth))
                        {
                            if_constexpr(SUPPORT_POSITIONAL_PARAMETERS)
                            {
                                // If the value is followed by a '$', treat it
                                // as param_index rather than as min_width.
                                if(*fmt == '$')
                                {
                                    param_index = value;
                                    // We may have also accidentally read '0' as zeropad flag, so clear it.
                                    fmt_flags &= ~fmt_zeropad;
                                    goto moreflags1;
                                }
                            }
                            min_width = value;
                        }
                        else
                        {
                            precision = value;
                        }
                        goto moreflags;
                    }

                    case '.': fmt_flags |= got_minwidth; goto moreflags1;
                    case '*':
                    {
                        // Read indirect min-width or precision
                        ++fmt;

                        unsigned opt_index = 0;
                        if_constexpr(SUPPORT_POSITIONAL_PARAMETERS)
                        {
                            //opt_index = read_param_index(fmt);
                            opt_index = read_int(fmt, 0);
                            if(opt_index) /*if(*fmt == '$')*/ ++fmt;
                        }
                        GET_ARG(int,v,0, opt_index, goto moreflags);
                        if(!(fmt_flags & got_minwidth))
                        {
                            min_width = (v < 0) ? -v : v;
                            fmt_flags |= fmt_leftalign * (v < 0); // negative value sets left-aligning
                        }
                        else
                        {
                            if(v >= 0) { precision = v; } // negative value is treated as unset
                        }
                        goto moreflags;
                    }

                    // Read possible length modifier.
                    case 't': if_constexpr(!SUPPORT_T_LENGTH) goto got_unk; else {
                              set_sizebase(base_decimal,std::ptrdiff_t);          goto moreflags1; }
                    case 'z': set_sizebase(base_decimal,std::size_t);             goto moreflags1;
                    case 'l': set_sizebase(base_decimal,long);  if(*++fmt != 'l') goto moreflags; PASSTHRU
                    case 'L': set_sizebase(base_decimal,long long);               goto moreflags1; // Or 'long double'
                    case 'j': if_constexpr(!SUPPORT_J_LENGTH) goto got_unk; else {
                              set_sizebase(base_decimal,std::intmax_t);           goto moreflags1; }
                    case 'h': if_constexpr(!SUPPORT_H_LENGTHS) goto got_unk; else {
                              set_sizebase(base_decimal,short); if(*++fmt != 'h') goto moreflags; /*PASSTHRU*/
                              set_sizebase(base_decimal,char);                    goto moreflags1; }

                    // Read the format type

                    // %n format
                    case 'n':
                    if_constexpr(SUPPORT_N_FORMAT)
                    {
                        GET_ARG(void*,pointer,3, param_index, continue);

                        auto value = state.param - param;
                    #if defined(__ARMEL__) || defined(__i386) || defined(__x86_64) || __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN
                        // This loop is a great deal shorter code, but slower (does not matter
                        // really for %n), and endian sensitive (that's what the #ifdef is for).
                        for(unsigned m=get_type(), n=0; n<m; ++n, value>>=8)
                            ((unsigned char*)pointer)[n] = value;
                    #else
                        if(!is_type(int))
                        {
                            if(sizeof(int) != sizeof(long) && is_type(long))  { *static_cast<long*>(pointer) = value; }
                            else if(SUPPORT_H_LENGTHS && sizeof(int) != sizeof(short)
                                 && is_type(short))                           { *static_cast<short*>(pointer) = value; }
                            else if(SUPPORT_H_LENGTHS && sizeof(int) != sizeof(char)
                                 && is_type(char))                            { *static_cast<signed char*>(pointer) = value; }
                            else /*if(sizeof(long) != sizeof(long long)
                                 && is_type(long long))*/                     { *static_cast<long long*>(pointer) = value; }
                        }
                        else                                                  { *static_cast<int*>(pointer) = value; }
                    #endif
                        continue; // Nothing to format
                    } else goto got_unk;

                    // String format
                    case 's':
                    {
                        GET_ARG(void*,pointer,3, param_index, continue);

                        source = static_cast<const char*>(pointer);
                        if(source)
                        {
                            length = std::strlen(source);
                            // Only calculate length on non-null pointers
                        }
                        else
                        {
                            fmt_flags |= (PFX_MUL*prefix_null);
                        }

                        // precision is treated as maximum width
                        break;
                    }

                    // Character format
                    case 'c':
                    {
                        GET_ARG(int,c,0, param_index, continue);

                        state.append(numbuffer,0); //state.flush();

                        numbuffer[0] = static_cast<char>(c);
                        length = 1;
                        if_constexpr(STRICT_COMPLIANCE)
                        {
                            precision = ~0u; // No max-width
                        }
                        break;
                    }

                    // Pointer and integer formats
                    case 'p': { fmt_flags |= fmt_pointer;  set_sizebase(base_hex, void*); goto got_int; }
                    case 'X': { fmt_flags |= fmt_ucbase; } PASSTHRU
                    case 'x': {                            set_base(base_hex);   goto got_int; }
                    case 'o': {                            set_base(base_octal); goto got_int; }
                    case 'b': if_constexpr(!SUPPORT_BINARY_FORMAT) goto got_unk;
                              else                       { set_base(base_binary); goto got_int; }
                    case 'd': case 'i': { fmt_flags |= fmt_signed; } PASSTHRU
                    case 'u': got_int:
                    {
                        intfmt_t value = 0;

                        if_constexpr(SUPPORT_H_LENGTHS)
                        {
                            uintfmt_t uvalue;

                            if(sizeof(long) != sizeof(long long) && is_type(long long)) { GET_ARG(unsigned long long,v,2, param_index, continue); uvalue = v; }
                            else if(sizeof(int) != sizeof(long) && is_type(long))       { GET_ARG(unsigned long,v,1, param_index, continue); uvalue = v; }
                            else                                                        { GET_ARG(unsigned int,v,0, param_index, continue); uvalue = v; }

                            unsigned m = 8*get_type();
                            if(m < 8*sizeof(value))
                            {
                                // Remove extra bits in case it's necessary (shorts and chars only)
                                uintfmt_t mask = (uintfmt_t(1) << m);
                                uvalue &= (mask-1);
                                // Sign-extend if necessary
                                if(fmt_flags & fmt_signed)
                                {
                                    mask >>= 1;
                                    uvalue = (uvalue ^ mask) - mask;
                                }
                            }
                            value = uvalue;
                        }
                        else // No support for h/hh lengths
                        {
                            intfmt_t svalue;

                            if(sizeof(long) != sizeof(long long) && is_type(long long)) { GET_ARG(long long,v,2, param_index, continue); svalue = v; }
                            else if(sizeof(int) != sizeof(long) && is_type(long))       { GET_ARG(long,v,1, param_index, continue); svalue = v; }
                            else                                                        { GET_ARG(int,v,0, param_index, continue); svalue = v; }

                            // Remove extra bits if necessary (zero-extend)
                            unsigned m = 8*get_type();
                            if((m < 8*sizeof(value)) && !(fmt_flags & fmt_signed))
                            {
                                uintfmt_t mask = ((uintfmt_t(1) << m)-1);
                                svalue &= mask;
                            }
                            value = svalue;
                        }

                        unsigned min_digits = 1;
                        if(precision != ~0u)
                        {
                            if_constexpr(STRICT_COMPLIANCE) { fmt_flags &= ~fmt_zeropad; }
                            min_digits = precision;
                            precision = ~0u; // No max-width
                        }

                        // Run flush() before we overwrite prefixbuffer/numbuffer,
                        // because putbegin/putend can still refer to that data at this point
                        state.append(numbuffer,0); //state.flush();

                        std::tie(length,fmt_flags) = format_integer(numbuffer, value, fmt_flags, min_digits);
                        break;
                    }

                    case 'A': PASSTHRU
                    case 'a': if_constexpr(!SUPPORT_FLOAT_FORMATS || !SUPPORT_A_FORMAT) goto got_unk; else {
                              set_base(base_hex);
                              if(precision == ~0u) { } /* TODO: set enough precision for exact representation */
                              fmt_flags |= fmt_exponent;
                              goto got_flt; }
                    case 'E': PASSTHRU
                    case 'e': if_constexpr(!SUPPORT_FLOAT_FORMATS) goto got_unk; else {
                              // Set up 'e' flags
                              fmt_flags |= fmt_exponent; // Mode: Always exponent
                              goto got_flt; }
                    case 'G': PASSTHRU
                    case 'g': if_constexpr(!SUPPORT_FLOAT_FORMATS) goto got_unk; else {
                              // Set up 'g' flags
                              fmt_flags |= fmt_autofloat; // Mode: Autodetect
                              goto got_flt; }
                    case 'F': PASSTHRU
                    case 'f': if_constexpr(!SUPPORT_FLOAT_FORMATS) goto got_unk; got_flt:;
                    if_constexpr(SUPPORT_FLOAT_FORMATS)
                    {
                        fmt_flags |= fmt_ucbase * (~*fmt & 0x20) / 0x20; // for capital letters

                        // Run flush() before we overwrite prefixbuffer/numbuffer,
                        // because putbegin/putend can still refer to that data at this point
                        state.append(numbuffer,0); //state.flush();

                        if(precision == ~0u) precision = 6;
                        if(SUPPORT_LONG_DOUBLE && is_type(long long))
                        {
                            GET_ARG(long double,value,5, param_index, continue);
                            std::tie(length,fmt_flags) = format_float(numbuffer, value, fmt_flags, precision);
                        }
                        else
                        {
                            GET_ARG(double,value,4, param_index, continue);
                            std::tie(length,fmt_flags) = format_float(numbuffer, value, fmt_flags, precision);
                        }
                        precision = ~0u; // No max-width
                        break;
                    } else break;
                    /* f,F: [-]ddd.ddd
                     *                     Recognize [-]inf and nan (INF/NAN for 'F')
                     *      Precision = Number of decimals after decimal point (assumed 6)
                     *
                     * e,E: [-]d.ddde+dd
                     *                     Exactly one digit before decimal point
                     *                     At least two digits in exponent
                     *
                     * g,G: Like e, if exponent is < -4 or >= precision
                     *      Otherwise like f, but
                     *
                     * a,A: [-]0xh.hhhhp+d  Exactly one hex-digit before decimal point
                     *                      Number of digits after it = precision.
                     */

                }
                state.format_string(source, length, min_width, precision, fmt_flags);

                #undef GET_ARG
            }
        unexpected:;
            // Format string processing is complete.
            if_constexpr(!SUPPORT_POSITIONAL_PARAMETERS)
            {
                goto exit_rounds;
            }
            else
            {
                // Do book-keeping after each round. See notes in the beginning of this function.
                unsigned n_params = std::max(round % MAX_AUTO_PARAMS, round / POS_PARAM_MUL);
                unsigned rndno = (round / MAX_AUTO_PARAMS) % MAX_ROUNDS;
                unsigned paramdata_units  = n_params;
                unsigned paramsize_units = (n_params * sizeof(unsigned short) + largest-1) / largest;
                switch(rndno)
                {
                    case 3:
                    {
                        if(round / POS_PARAM_MUL == 0)
                        {
                            // No positional parameters, jump to round 0
                            round = 0;
                            continue;
                        }
                        // Allocate room for offsets and parameters in one go.
                        //printf("%u params, sizesize=%u datasize=%u largest=%zu\n", n_params, paramsize_size, paramdata_size, largest);
                        param_data_table = auto_dealloc_pointer<SUPPORT_POSITIONAL_PARAMETERS>::type(
                            new unsigned char[largest * (paramsize_units + paramdata_units)]);
                        // It is likely we allocated too much (for example if all parameters are ints),
                        // but this way we only need one allocation for the entire duration of the printf.
                        break;
                    }
                    case 2:
                    {
                        unsigned char* table = &param_data_table[0];
                        unsigned short* param_offset_table = reinterpret_cast<unsigned short *>(table);
                        for(unsigned n=0; n<n_params; ++n)
                        {
                            // Convert the size & typecode into an offset
                            unsigned /*size = param_offset_table[n]/8,*/ typecode = param_offset_table[n]/*%8*/;
                            unsigned offset = n + paramsize_units;
                            param_offset_table[n] = offset;
                            // Load the parameter and store it
                            unsigned char* tgt = &table[largest * offset];
                            switch(typecode)
                            {
                                case 0: { type0:; *(int*)tgt = va_arg(ap,int);     break; }
                                case 1: { *(long*)tgt      = va_arg(ap,long);      break; }
                                case 2: { *(long long*)tgt = va_arg(ap,long long); break; }
                                case 3: { *(void**)tgt     = va_arg(ap,void*);     break; }
                                case 4: if_constexpr(!SUPPORT_FLOAT_FORMATS) { goto unreach; } else
                                        { *(double*)tgt    = va_arg(ap,double);    break; }
                                case 5: if_constexpr(!SUPPORT_FLOAT_FORMATS || !SUPPORT_LONG_DOUBLE) { goto unreach; } else
                                        { *(long double*)tgt = va_arg(ap,long double); break; }
                                #ifdef __GNUC__
                                default: unreach: __builtin_unreachable(); goto type0;
                                #else
                                default: unreach: goto type0;
                                #endif
                            }
                        }
                        break;
                    }
                    #ifdef __GNUC__
                    case 1: case 0: goto exit_rounds;
                    default: __builtin_unreachable();
                    #else
                    default: goto exit_rounds;
                    #endif
                }
                // Proceed to next round (round 2 or round 1)
                round = (rndno-1) * MAX_AUTO_PARAMS;
            }
        }
    exit_rounds:;
        state.flush();
        return state.param - param;
    }

    #undef set_sizebase
    #undef set_base
    #undef get_base
    #undef get_type
    #undef is_type
    #undef BASE_MUL
    #undef FLAG_MUL
}
}
#ifdef __GNUC__
 #pragma GCC pop_options
#endif

extern "C" {
    static void wfunc(char*, const char* src, std::size_t n)
    {
        /* PUT HERE YOUR CONSOLE-PRINTING FUNCTION */
        extern int _write(int file, char *data, int len);
        _write(0, (char *) src, n);
    }

    int __wrap_printf(const char* fmt, ...) USED_FUNC;
    int __wrap_printf(const char* fmt, ...)
    {
        std::va_list ap;
        va_start(ap, fmt);
        int ret = myprintf::myvprintf(fmt, ap, nullptr, wfunc);
        va_end(ap);
        return ret;
    }

    int __wrap_vprintf(const char* fmt, std::va_list ap) USED_FUNC;
    int __wrap_vprintf(const char* fmt, std::va_list ap)
    {
        return myprintf::myvprintf(fmt, ap, nullptr, wfunc);
    }

#ifdef SUPPORT_FILE_FUNCTIONS
    int __wrap_vfprintf(std::FILE*, const char* fmt, std::va_list ap) USED_FUNC;
    int __wrap_vfprintf(std::FILE*, const char* fmt, std::va_list ap)
    {
        return myprintf::myvprintf(fmt, ap, nullptr, wfunc);
    }

    int __wrap_fprintf(std::FILE*, const char* fmt, ...) USED_FUNC;
    int __wrap_fprintf(std::FILE*, const char* fmt, ...)
    {
        std::va_list ap;
        va_start(ap, fmt);
        int ret = myprintf::myvprintf(fmt, ap, nullptr, wfunc);
        va_end(ap);
        return ret;
    }

  #ifdef SUPPORT_FIPRINTF
    //int __wrap_fiprintf(std::FILE*, const char* fmt, ...) USED_FUNC;
    int __wrap_fiprintf(std::FILE*, const char* fmt, ...)
    {
        std::va_list ap;
        va_start(ap, fmt);
        int ret = myprintf::myvprintf(fmt, ap, nullptr, wfunc);
        va_end(ap);
        return ret;
    }
  #endif
#endif

    int __wrap_vsprintf(char* target, const char* fmt, std::va_list ap) USED_FUNC;
    int __wrap_vsprintf(char* target, const char* fmt, std::va_list ap)
    {
        typedef void (*afunc)(char*,const char*,std::size_t);

        int ret = myprintf::myvprintf(fmt, ap, target, (afunc)std::memcpy);
        target[ret] = '\0';
        return ret;
    }

    int __wrap_sprintf(char* target, const char* fmt, ...) USED_FUNC;
    int __wrap_sprintf(char* target, const char* fmt, ...)
    {
        std::va_list ap;
        va_start(ap, fmt);
        int ret = __wrap_vsprintf(target, fmt, ap);
        va_end(ap);
        return ret;
    }

#ifdef SUPPORT_SNPRINTF
    static thread_local char* snprintf_cap = nullptr;
    static void memcpy_cap(char* target, const char* source, std::size_t count)
    {
        if(target < snprintf_cap)
            std::memcpy(target, source, std::min(count, std::size_t(snprintf_cap-target)));
    }

    int __wrap_vsnprintf(char* target, std::size_t limit, const char* fmt, std::va_list ap) USED_FUNC;
    int __wrap_vsnprintf(char* target, std::size_t limit, const char* fmt, std::va_list ap)
    {
        auto oldcap = snprintf_cap; // Backup the global variable to satisfy re-entrancy
        snprintf_cap = target + limit;
        int ret = myprintf::myvprintf(fmt, ap, target, memcpy_cap);
        snprintf_cap = oldcap;      // Restore backup
        if(limit) target[std::min(limit-1, std::size_t(ret))] = '\0';
        return ret;
    }

    int __wrap_snprintf(char* target, std::size_t limit, const char* fmt, ...) USED_FUNC;
    int __wrap_snprintf(char* target, std::size_t limit, const char* fmt, ...)
    {
        std::va_list ap;
        va_start(ap, fmt);
        int ret = __wrap_vsnprintf(target, limit, fmt, ap);
        va_end(ap);
        return ret;
    }
#endif

#ifdef SUPPORT_ASPRINTF
    static void donothing(char*,const char*,std::size_t){}

    //int __wrap_vasprintf(char** target, const char* fmt, va_list ap1) USED_FUNC;
    int __wrap_vasprintf(char** target, const char* fmt, va_list ap1)
    {
        std::va_list ap2;
        va_copy(ap2, ap1);
        int ret = myprintf::myvprintf(fmt, ap2, nullptr, donothing);
        va_end(ap2);
        *target = (char *) std::malloc(ret + 1);
        ret = __wrap_vsprintf(*target, fmt, ap1);
        return ret;
    }

    //int __wrap_asprintf(char** target, const char* fmt, ...) USED_FUNC;
    int __wrap_asprintf(char** target, const char* fmt, ...)
    {
        std::va_list ap;
        va_start(ap, fmt);
        int ret = __wrap_vasprintf(target, fmt, ap);
        va_end(ap);
        return ret;
    }
#endif

    int __wrap_puts(const char* str) USED_FUNC;
    int __wrap_puts(const char* str)
    {
        return __wrap_printf("%s\r\n", str); // %s\r\n
    }

#ifdef SUPPORT_FILE_FUNCTIONS
    //int __wrap_fflush(std::FILE*) USED_FUNC;
    int __wrap_fflush(std::FILE*)
    {
        return 0;
    }

    int __wrap_fputs(std::FILE*, const char* str) USED_FUNC;
    int __wrap_fputs(std::FILE*, const char* str)
    {
        return __wrap_puts(str);
    }


    int __wrap_fwrite(void* buffer, std::size_t a, std::size_t b, std::FILE*) USED_FUNC;
    int __wrap_fwrite(void* buffer, std::size_t a, std::size_t b, std::FILE*)
    {
        wfunc(nullptr, (const char*)buffer, a*b);
        return a*b;
    }

    //int __wrap_fputc(int c, std::FILE*) USED_FUNC;
    int __wrap_fputc(int c, std::FILE*)
    {
        char ch = c;
        wfunc(nullptr, &ch, 1);
        return c;
    }
#endif

    int __wrap_putchar(int c) USED_FUNC;
    int __wrap_putchar(int c)
    {
        char ch = c;
        wfunc(nullptr, &ch, 1);
        return c;
    }

}/*extern "C"*/
