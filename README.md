# threadshuffle
A simple tool to test thread safety at set points in functions.

Set the points by using the DEBUG_SHUFFLE macro.
Call shuffle with a std::function<void()> and the amount of threads to run in parallel.
To run multiple functions at the same time use unique_shuffle with a vector of functions instead.
In this case each function only gets called once, note that this will generate exponentially more permutations than using the duplicate shuffle function.

threadshuffle.h is the main header, see testshuffle.cxx for a possible implementation. (uses threadshuffle.cxx and shuffler.cxx implementation files)
permute.h can be used on its own, see testpermute.cxx for a possible implementation.

Library functionality is still a work in progress.
