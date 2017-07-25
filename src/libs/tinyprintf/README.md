# tinyprintf

`printf-c.cc` is a C++ module that replaces certain C-language libc functions
with a tiny alternatives suitable for embedded programs.

The functions replaced are
`printf`,
`vprintf`,
`sprintf`,
`vsprintf`,
`puts`,
`putchar`,
`fprintf`¹,
`vfprintf`¹,
`fiprintf`¹ ⁴,
`fputs`¹,
`fputc`¹,
`fflush`¹,
`fwrite`¹,
`snprintf`²,
`vsnprintf`²,
`asprintf`³, and
`vasprintf`³,

* ¹) Only if SUPPORT_FILE_FUNCTIONS is #defined
* ²) Only if SUPPORT_SNPRINTF is #defined
* ³) Only if SUPPORT_ASPRINTF is #defined
* ⁴) Only if SUPPORT_FIPRINTF is #defined

To use, compile `printf-c.cc` using your C++ compiler, and link it into your project.
You will have to add the following linker flags:

    -Wl,--wrap,printf  -Wl,--wrap,fprintf  -Wl,--wrap,sprintf   -Wl,--wrap,asprintf    
    -Wl,--wrap,vprintf -Wl,--wrap,vfprintf -Wl,--wrap,vsprintf  -Wl,--wrap,vasprintf    
    -Wl,--wrap,puts    -Wl,--wrap,putchar  -Wl,--wrap,snprintf  -Wl,--wrap,fwrite    
    -Wl,--wrap,fputs   -Wl,--wrap,fputc    -Wl,--wrap,vsnprintf -Wl,--wrap,fflush    
    -Wl,--wrap,fiprintf

What `-Wl,--wrap,somefunc` does it replaces all calls to `somefunc` with calls to `__wrap_somefunc`,
and all calls to `__real_somefunc` with calls to the original `somefunc`.
It is handy when you need to replace a library function that is not declared “weak”.

GNU build tools are probably required.

Compiling with `-ffunction-sections -fdata-sections` and linking with `-Wl,--gc-sections` is strongly recommended.
Compiling with `-flto` is supported.
Compiling with `-fbuiltin` is supported.

*IMPORTANT*: You will have to edit printf-c.cc file, locate `wfunc`,
and replace that function with code that is suitable for your project.

## Format string

The format string format is fully compliant with C99 and C++14,
with positional parameters supported optionally according to SUSv2:

* `"%%"` or
* `"%" [position-specifier] [flag ...] [minimum-width-specifier] ["." precision-specifier] [length-modifier] [format-type]`

Where
* `position-specifier` is `decimal-digit [ decimal-digit ... ] "$"`
  * Only supported and parsed if SUPPORT_POSITIONAL_PARAMETERS is set
* `minimum-width-specifier` and `precision-specifier` are `decimal-digit [ decimal-digit ... ]` or `"*" [position-specifier]`
  * The maximum minimum-width-specifier is 134217727 characters (2²⁷−1)
  * The maximum precision-specifier applied to non-numeric conversions (cutting width) is 134217727 characters (2²⁷−1)
  * The maximum precision-specifier applied to numeric conversions (minimum digits) is 22 digits, or 64 if SUPPORT_BINARY_FORMAT is set
  * Width/precision specifiers are ignored for the `"n"` format type
  * If STRICT_COMPLIANCE is set, precision specifier is ignored for `"c"` format type.
* `flag` is zero or more of these letters, in any order: `"+" | "-" | "0" | "#" | " "`
  * `"+"` specifies that a plus sign should be printed in front of a non-negative number. Only affects decimal non-unsigned numeric formats.
  * `" "` specifies that a space should be printed in front of a non-negative number. Only affects decimal non-unsigned numeric formats. Ignored if `"+"` also given.
  * `"-"` specifies that when padding to satisfy the minimum-width-specifier, the value should be left-aligned, not right-aligned.
  * `"0"` specifies that when padding to satisfy the minimum-width-specifier, the value should be padded with zeroes, not spaces. Ignored if `"-"` also given, or if a precision-specifier is used and STRICT_COMPLIANCE is set. Behavior is undefined if applied to non-numeric formats.
  * "`#"` specifies that "0x" or "0X" should be printed in front of non-zero hexadecimal numbers, and that an octal number must always begin with zero, even if precision of 0 is explicitly specified. Ignored if STRICT_COMPLIANCE is unset.
  * `"'"` flag, defined by SUSv2, is not supported
  * `"I"` flag, defined by glibc, is not supported
  * Flags are ignored for the `"n"` format type.
* `length-modifier` is `"h" | "hh" | "l" | "ll" | "L" | "j" | "z" | "t"`
  * `"h"` and `"hh"` are only supported if SUPPORT_H_LENGTHS is set
  * `"t"` is only supported if SUPPORT_T_LENGTH is set
  * `"j"` is only supported if SUPPORT_J_LENGTH is set
  * `"ll"` and `"L"` affect float formats only if SUPPORT_LONG_DOUBLE is set
  * Length modifiers are ignored for non-numeric formats ("s", "c") and the pointer format ("p"). I.e. `wchar_t` strings or `wint_t` chars are not supported.
* `format-type` is `"n" | "s" | "c" | "p" | "x" | "X" | "o" | "d" | "u" | "i" | "a" | "A" | "e" | "E" | "f" | "F" | "g" | "G" | "b"`
  * `"n"` is only supported if SUPPORT_N_FORMAT is set
  * `"b"` is only supported if SUPPORT_BINARY_FORMAT is set
  * `"e"`, `"E"`, `"f"`, `"F"`, `"g"`, and `"G"` are only supported if SUPPORT_FLOAT_FORMATS is set
  * `"a"` and `"A"` are only supported if SUPPORT_FLOAT_FORMATS and SUPPORT_A_FORMAT are both set
  * `"d"` and `"i"` are equivalent and have the same meaning
  * `"p"` is treated similarly as `%#x`, except that if STRICT_COMPLIANCE is set, `"+"` and `" "` flags will also be processed (this is what glibc does)
  * A null pointer passed to `"p"` format is printed as “`(nil)`” (note that contrary to glibc, this string may be cut by precision-specifier)
  * A null pointer passed to `"s"` format is printed as “`(null)`” (note that contrary to glibc, this string may be cut by precision-specifier)
  * Format types `"S"` and `"C"`, defined by SUSv2, are not supported
  * Format type `"m"`, defined by glibc, is not supported

## Features

* Design is optimized for code size
* Standards-compliant (C99 / C++11), see above for details
* Memory usage is negligible (around 30-200 bytes of automatic storage used, depending on compiler optimizations, register pressure and spilling, and whether binary formats are enabled)
  * If positional parameters are enabled and used, a dynamically allocated array is used to temporarily hold parameter information. The size of the array is directly proportional to the number of printf parameters. Each parameter takes about 10 bytes of memory (assuming the largest supported parameter is 64 bits wide).
* Re-entrant code (e.g. it is safe to call `sprintf` within your stream I/O function invoked by `printf`)
* Thread-safe as long as your wfunc is thread-safe. `printf` calls are not locked, so prints from different threads can interleave.
* Compatible with GCC’s optimizations where e.g. `printf("abc\n")` is automatically converted into `puts("abc")`
* Positional parameters are fully supported (e.g. `printf("%2$s %1$0*3$ld", 5L, "test", 4);` works and prints “test 0005”), disabled by default

## Caveats

* Stream I/O errors are not handled
* No buffering; all text is printed as soon as available, resulting in multiple calls of the I/O function (but as many bytes are printed with a single call as possible)
* No file I/O: printing is only supported into a predefined output (such as through serial port), or into a string. Any `FILE*` pointer parameters are completely ignored
* String data is never copied. Any pointers into strings are expected to be valid throughout the call to the printing function
* `dprintf`, `vdprintf` are not supported (POSIX.1-2008)
* `wprintf`, `fwprintf`, `swprintf`, `vwprintf`, `vfwprintf`, and `vswprintf` are not supported (C99, C++98)
* `printf_s`, `fprintf_s`, `sprintf_s`, `snprintf_s`, `vprintf_s`, `vfprintf_s`, `vsprintf_s`, `vsnprintf_s`, `wprintf_s`, `fwprintf_s`, `swprintf_s`, `snwprintf_s`, `vwprintf_s`, `vfwprintf_s`, `vswprintf_s`, and `vsnwprintf_s` are not supported (C11).
* Behavior differs to GNU libc printf when a nul pointer is printed with `p` or `s` formats and max-width specifier is used
* If positional parameters are enabled, there may be a maximum of 1023 parameters to printf.⁵
* `vsnprintf` and `snprintf` are thread-safe only if your compiler honors the `thread_local` attribute.
* If positional parameters are enabled, the format string can be scanned up to three times. If positional parameters are enabled but not used in the format string, no dynamic allocation occurs, but the format string is still scanned twice.

⁵) If you really need more than this, edit MAX_EXPLICIT_PARAMS and MAX_AUTO_PARAMS in printf-c.cc. Their product should be less 2³²/MAX_ROUNDS.

## Known bugs

* Floating point support (formats `e`, `E`, `f`, `F`, `g`, `G`, `a`, and `A`) is all sorts of broken and disabled by default

## Rationale

* This module was designed for use with mbed-enabled programming, and to remove any dependencies to stdio (specifically FILE stream facilities) in the linkage, reducing the binary size.
  * And perhaps a tiny bit of “we do what we must, because we can”.

## Author

Copyright © Joel Yliluoma 2017. (http://iki.fi/bisqwit/)    
Distribution terms: MIT
