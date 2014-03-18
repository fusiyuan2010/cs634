#include "WordTransformer.h"
#include <cctype>
#include <algorithm>
#include <utility>
#include <cstring>


/* In stem(p,i,j), p is a char pointer, and the string to be stemmed is from
   p[i] to p[j] inclusive. Typically i is zero and j is the offset to the last
   character of a string, (p[j+1] == '\0'). The stemmer adjusts the
   characters p[i] ... p[j] and returns the new end-point of the string, k.
   Stemming never increases word length, so i <= k <= j. To turn the stemmer
   into a module, declare 'stem' as extern, and delete the remainder of this
   file.
*/

extern int stem(char * p, int i, int j);

std::string SimpleWordTransformer::word_to_upper(const std::string &word) {
    std::string result;
    for(size_t i = 0; i < word.size(); i++) {
        if (word[i] == '\'')
            break;
        result.append(1, toupper(word[i]));
    }
    return result;
}

std::string SimpleWordTransformer::Transform(const std::string &word) {
    std::string result = word_to_upper(word);
    int nonalpha_count = 0;
    for(const auto &c : result) {
        if (!isalpha(c)) {
            /* too special char inside, omit it */
            if (!isdigit(c) && c != '\'' && c != '-' && c != '&')
                return "";
            nonalpha_count++;
        }
    }
    /* contains too much non_alpha char */
    if (nonalpha_count > 0.3 * result.size())
        return "";

    if (function_words_.count(result) > 0)
        return "";

    if (!isalpha(word[0]))
        return "";

    /* stemming */
    char *tmpbuf = new char[word.size() + 1];
    strcpy(tmpbuf, word.c_str());
    tmpbuf[stem(tmpbuf, 0, word.size() - 1) + 1] = '\0';
    result = word_to_upper(tmpbuf);
    delete[] tmpbuf;

    if (function_words_.count(result) > 0)
        return "";
    /* 
    stemming::english_stem<> StemEnglish;
    wchar_t wbuf[64];
    wmemset(wbuf, 0, 64);
    mbstowcs(wbuf, word.c_str(), word.size());
    std::wstring wword(wbuf);
    StemEnglish(wword);

    char s[64];
    wcstombs(s, wword.c_str(), std::min(int(word.size()), 63));
    return s;
    */
    return result;
}

int SimpleWordTransformer::ReadBlacklist(const char *filename) 
{
    FILE *f = fopen(filename, "rt");
    if (f == NULL)
        return -1;

    char buf[32];
    while(fgets(buf, 32, f) != NULL) {
        size_t i = strlen(buf);
        if (i > 0) {
            buf[i - 1] = '\0';
            function_words_.insert(buf);
        }
    }
    printf("Read black word list %lu items\n", function_words_.size());
    fclose(f);
    return 0;
}

