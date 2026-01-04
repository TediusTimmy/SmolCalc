SlowFloat
=========

SlowFloat is a very simple single-precision decimal floating point library. The author got tired of ((0.1 * 10.0) != 1.0) evaluating true.

SlowFloat has nine decimal digits of precision. It uses just C++ with no inline assembly. It just requires uint64_t to be a type. Yes, it is slow, but it also gets rid of certain idiosyncrasies in doing math with a computer.

Notes:
* The exponent range is -32767 to 32767 inclusive. (Note that Backwards performs several operations by converting to double, so many functions don't support the full range. In fact: for the exponentiation operator, 2^1023 is the last result before returning infinity, while 2^108852 can be computed using a loop and only multiplications.)
* There are no subnormal numbers. As the exponent range is nearly as large as octuple precision, I didn't think they were necessary.
* The IEEE-754 exceptions return their default values, as users of float/double in most languages would expect. Default replacements are not tuneable.
* SlowFloat has no signaling NaNs.
