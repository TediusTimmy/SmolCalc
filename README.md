SmolCalc
========

I took SQLite out of WTFITS and made this. It is back to memory a spreadsheet, and I made it smol. There are only 702 columns and 100000 rows.  
I can now fill every cell without my computer crashing.


Invocation
----------
` SmolCalc [-0 | -1 | -2 | -3 | -4 | -5 | -6 | -7] { -l <library name> } { -b <batch string> } [ -i <import csv> ] [ <load file> [ <save file> ] ] `

The first argument is the number system to use. There are currently seven implemented:
* `-0` BC number-like system used in BC-DeciCalc
* `-1` Decimal floating point used in DeciCalc
* `-2` The SlowFloat decimal floating point system used in the Backrooms engine
* `-3` Normal machine double
* `-4` libmpdec used by Python for decimal floating point
* `-5` mpfr with quasi-decimal precision
* `-6` DAPFP - Doubly-arbitrary precision floating point (decimal)
* `-7` Decimal floating point used in X16Cell

Note for libraries: when you load a library, it gets stored in the working sheet. You only need to load them once.


Number System notes
-------------------
Half of these (`-1`, `-2`, `-3`, and `-7`) are fixed precision: changing the working precision has no effect. Most of these lack some of the rounding modes provided by the BC number-like system: `-1` and `-2` lack the double rounding mode; `-4` lacks ties-to-odd; and `-3` and `-5` only have ties-to-even, to-positive-infinity, to-negative-infinity, and to-zero (and `-5` has away-from-zero). Also also, be prepared for strange results when you change the rounding mode in `-3`: most C runtimes are broken when you change the machine rounding mode. Mode `-7` only rounds to zero.

Also note that `-6` (and to some extent `-4`) can handle exponents the rest of the code considers unreasonable and can make `-0` crash. The mpfr backend won't even reach 32-bit exponents. The libmpdec backend allows full 64-bit exponents. And DAPFP takes a lot of code out of its range of validity.

Manual
------
See [Le Manuel](Manuel.md).
