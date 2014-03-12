#ifndef _WORDTRANSFORMER_H_
#define _WORDTRANSFORMER_H_

#include "Parser.h"
#include <unordered_set>
#include <string>

class SimpleWordTransformer : public WordTransformer
{
    std::unordered_set<std::string> function_words_;
    std::string word_to_upper(const std::string &word);

public:
    std::string Transform(const std::string &word); 
    int ReadBlacklist(const char *filename);
};

#endif

