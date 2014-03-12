#ifndef _PARSER_H
#define _PARSER_H

#include <string>
#include <vector>

class WordTransformer
{
public:
    /* transform it or */
    /* return empty string if the word should be filtered */
    virtual std::string Transform(const std::string &word) {
        return word;
    }

    virtual ~WordTransformer() {}
};


class ArticleHandler
{
public:
    virtual void AddArticle(const std::vector<std::string> &title,
            const std::vector<std::string> &article,
            time_t timestamp)
    {
    }

    virtual ~ArticleHandler() {}
};

class Parser
{
private:
    std::vector<std::string> cur_title_;
    std::vector<std::string> cur_article_;
    time_t cur_timestamp_;
    ArticleHandler *article_handler_;
    WordTransformer *word_transformer_;

    enum State {
        S_NONE,
        S_IN_CONTENT,
    } state_;

public:
    Parser(ArticleHandler *handler, WordTransformer *transformer) 
        : article_handler_(handler), word_transformer_(transformer), state_(S_NONE)
    {
    }

    void ParseLine(const char *str);

private:
    void add_title(const char *str);
    void add_content(const char *str);
};

#endif

