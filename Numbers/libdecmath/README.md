libdecmath
==========

A library for doing decimal math. This library is C code implementing a decimal floating point type inspired by the IEEE 754-2008 decimal64 floating point format. It also has 16 digits of precision: while it is not binary compatible with the decimal64 format, it ought to produce the same results for calculations that do not have overflows to infinity or underflow to subnormals. The only really odd thing about the representation is that setting it to all zero bits gives the number 1.0.

The library requires 64 bit math, and the type uint64_t to be defined. The underlying type of the data is uint64_t, so a dm_double can be passed around like normal doubles and integers in code. It will also try to use a 128 bit type, if it believes it to be available. The library includes a stripped-down sub-library for doing 128 bit calculations, should an 128 bit integral type not be available. Define DM_NO_128_BIT_TYPE to use that instead.

You can define DM_NO_DOUBLE_MATH to not get conversions to/from double, and DM_NO_LONG_DOUBLE_MATH to not get conversions to/from 'long double'. Note that no double conversions implies no 'long double' conversions. The double conversion tests fail on the Cygwin MingW because 1e100 is one ULP from what I have to assume is the "right" answer. Also 1e300 is three ULPs away.

Unit tests have 99.7% line and branch coverage (though I don't have a tool that computes condition coverage). I'm fairly certain that the two lines and two branches cannot be exercised by that data type, but oh well. The code that is present is more likely to be right, though it's not a guarantee. While unit tests can expose when code OUGHT to be present, but isn't, it can't test for it. Someone making up cases can find logic that isn't present, but unit tests can't cover lines that don't yet exist. And then you have code that is technically equivalent, but produces different numerical results, and one of those results feels superior to other results.  
Take the bug fix: when subtracting two numbers and the difference of the exponents was the total precision plus one (17), and the operation canceled so that the guard digit was needed, then the least significant digit was always rounded away from zero, even when it shouldn't have been. This case came up while I was trying to figure out how to implement addition as it is done in binary: with only three extra bits. I use two digits, but that still keeps the math in the realm of 64 bit operations.  
Another bug: when the difference of the exponents was one and there was drastic cancellation, the result of subtraction would have an exponent that was off by one (found while implementing fma).

Differences from IEEE-754:
* It doesn't support floating point exceptions.
* It doesn't have gradual underflow. The range of the exponent is larger to make up for this deficiency.
* They cannot be sorted using integer compares. The sign is stored in the wrong place for that. (This may be an issue with decimal64, though.)

Note that it is a royal pain to debug code when the compiler doesn't understand the type of a variable.


MISRA version
-------------

The dm_double_m version has had the MISRA C standard applied to it. While I find the new functions to be slightly less readable, with the exception of dm_double_fpclassify, which has lower complexity, pretty much every function has the same complexity whether we return early or not. The only other function with a different complexity is dm_double_fromstring, and it is a different function entirely. However, the maintainability index of the MISRA version is lower. Fun times with useless metrics.

Of note is that, as MISRA doesn't like right-shifting a signed number, the exponent is now stored as a biased number also. And, if I was doing that, I could move the sign. This version is binary incompatible with the other version. However, now all bits set to zero is the number zero, numbers can be sorted using integer compares, and the sign bit is the sign bit.

Also note that printing a non-canonical number results in a leading "10" in the old code and a leading "0" in the MISRA version.
