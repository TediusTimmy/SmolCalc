   Tested this pattern, continuing until either the result became infinity, or I quit.  
=2  
=A1*A1  
=A2*A2  
...  

   Results:  
-0     27  
   I quit here, as the number is twenty million digits and the string is slow.  
   In fact, at this length, I'm sure there are threading issues that may lead to program hangs.  
-1     12  
-2     18  
   Despite only having nine digits of precision, this number system has a greater range: 2^6 times more range.  
-3     11  
-4     63  
-5     31  
-6     1000  
   I stopped here, with the exponent only 300 digits. I didn't bother to keep going.
