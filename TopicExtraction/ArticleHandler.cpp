#include "ArticleHandler.h"
#include <cstdio>
#include <cmath>

void SimpleArticleHandler::AddArticle(const std::vector<std::string> &title,
                                      const std::vector<std::string> &article,
                                      time_t timestamp)
{
    printf("Article %ld:\nTitle:", timestamp);
    for(const std::string &s : title) {
        printf("%s ", s.c_str());
    }
    printf("\nContent:");

    int acount = 0;
    for(const std::string &s : article) {
        printf("%s ", s.c_str());
        if (++acount == 20)
            break;
    }
    printf("\n");
}

void TFIDFArticleHandler::AddArticle(const std::vector<std::string> &title,
                                     const std::vector<std::string> &article,
                                     time_t timestamp)
{
    /* currently title is not used */
    /* skip too short useless articles */
    if (article.size() < 30)
        return;
    articles_.emplace_back(Article(title, article, timestamp));
}


void TFIDFArticleHandler::GetTFIDF() 
{
    for(auto &d : articles_) {
        int max_term_freq = 0;
        for(auto &t: d.terms_) {
            if (++d.term_freq_[t] > max_term_freq)
                max_term_freq = d.term_freq_[t];
        }

        for(const auto &t: d.term_freq_) {
            df_[t.first]++;
            d.term_weight_[t.first] = 0.5 + 0.5 * t.second / max_term_freq;
        }
    }

    for(auto &d : articles_) {
        for(auto &t : d.term_weight_) {
            if (df_[t.first] < 2)
                continue;
            t.second *= log(articles_.size() / df_[t.first]);
            d.tfidf_terms_[t.second] = t.first;
        }
    }
}

void TFIDFArticleHandler::ShowResult() {
    for(auto &d : articles_) {
        int i = 0;
        printf("==========================\nTime stamp: %ld\n", d.timestamp_);
        for(std::map<double, std::string>::const_reverse_iterator it = d.tfidf_terms_.rbegin();
                it != d.tfidf_terms_.rend(); ++it) {
            if (++i < 10)
                printf("%s : %lf\n", it->second.c_str(), it->first);
        }
    }
}

