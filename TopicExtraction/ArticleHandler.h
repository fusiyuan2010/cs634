#ifndef _ARTICLEHANDLER_H_
#define _ARTICLEHANDLER_H_
#include "Parser.h"
#include <string>
#include <unordered_map>
#include <map>

class SimpleArticleHandler: public ArticleHandler 
{
public:
    void AddArticle(const std::vector<std::string> &title,
        const std::vector<std::string> &article,
        time_t timestamp);
};


class TFIDFArticleHandler : public ArticleHandler {
    std::unordered_map<std::string, int> term_freq_;
    class Article {
    public:
        Article(const std::vector<std::string> &terms, const time_t &timestamp) 
            : terms_(terms), timestamp_(timestamp) {}
            
        std::vector<std::string> terms_;
        time_t timestamp_;
        std::unordered_map<std::string, int> term_freq_;
        std::unordered_map<std::string, double> term_weight_;
        std::map<double, std::string> tfidf_terms_;
    };

    std::vector<Article> articles_;
    std::unordered_map<std::string, int> df_;

public:
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

    const std::map<double, std::string>& GetArticleTFIDFTerms(size_t i) const
    {
        return articles_[i].tfidf_terms_;
    }

    void GetTFIDF();
    void ShowResult();
};


#endif

