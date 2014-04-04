#ifndef _ARTICLEHANDLER_H_
#define _ARTICLEHANDLER_H_
#include "Parser.h"
#include <string>
#include <unordered_map>
#include <map>
#include <cstdio>

class SimpleArticleHandler: public ArticleHandler 
{
public:
    void AddArticle(const std::vector<std::string> &title,
        const std::vector<std::string> &article,
        time_t timestamp);
};


class TFIDFArticleHandler : public ArticleHandler {
    std::unordered_map<std::string, int> term_freq_;
    int tfidf_fomula_;
    class Article {
    public:
        Article(const std::vector<std::string> &title,
                const std::vector<std::string> &terms,
                const time_t &timestamp) 
            : title_(title), terms_(terms), timestamp_(timestamp) {}
            
        std::vector<std::string> title_;
        std::vector<std::string> terms_;
        time_t timestamp_;
        std::unordered_map<std::string, int> term_freq_;
        std::unordered_map<std::string, double> term_weight_;
        std::multimap<double, std::string> tfidf_terms_;
        std::multimap<double, std::string> freq_terms_;
    };

    std::vector<Article> articles_;
    std::unordered_map<std::string, int> df_;

public:
    TFIDFArticleHandler(int tfidf_fomula)
        : tfidf_fomula_(tfidf_fomula) {}

    void AddArticle(const std::vector<std::string> &title,
            const std::vector<std::string> &article,
            time_t timestamp);

    size_t size() const
    {
        return articles_.size();
    }

    const std::vector<std::string>& GetArticle(size_t i) const
    {
        return articles_[i].terms_;
    }

    const std::vector<std::string>& GetTitle(size_t i) const
    {
        return articles_[i].title_;
    }

    const std::multimap<double, std::string>& GetArticleTFIDFTerms(size_t i) const
    {
        return articles_[i].tfidf_terms_;
    }

    const std::multimap<double, std::string>& GetArticleFreqTerms(size_t i) const
    {
        return articles_[i].freq_terms_;
    }

    const std::unordered_map<std::string, int>& GetArticleTermFreq(size_t i) const
    {
        return articles_[i].term_freq_;
    }

    void GetTFIDF();
    void ShowResult(FILE *f);
};


#endif

