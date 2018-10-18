#include <functional>
#include <vector>
#include "shuffler.h"
#include "permute.h"

//macro to be used in the user defined function
#define DEBUG_SHUFFLE if(local) local->wait();

//shuffle all possible permutations of the macro DEBUG_SHUFFLE
void shuffle(std::function<void()> target, int threads, int max_depth = 0);

//for different functions at once
void unique_shuffle(std::vector<std::function<void()>> &targets, int max_depth = 0);

