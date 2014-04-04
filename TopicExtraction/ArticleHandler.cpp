#include "ArticleHandler.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <utility>

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
    double avg_len = 0;
    for(const auto &t : article) {
        avg_len += t.size();
    }
    avg_len /= (double)article.size();
    if (article.size() < 10 || avg_len <= 4.5)
        return;
    articles_.emplace_back(Article(title, article, timestamp));
    int i = rand() % articles_.size();
    /* reorder */
    std::swap(articles_[i], articles_[articles_.size() - 1]);
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
            if (tfidf_fomula_ == 1)
                d.term_weight_[t.first] = t.second;
            else if (tfidf_fomula_ == 2)
                d.term_weight_[t.first] = 0.5 + 0.5 * t.second / max_term_freq;
            else 
                d.term_weight_[t.first] = 1;
            d.freq_terms_.insert(make_pair(t.second, t.first));
        }
    }

    for(auto &d : articles_) {
        for(auto &t : d.term_weight_) {
            if (tfidf_fomula_ != 3)  {
                if (df_[t.first] < 3)
                    continue;
                t.second *= log(articles_.size() / df_[t.first]);
            }
            d.tfidf_terms_.insert(make_pair(t.second, t.first));
        }
    }
}

void TFIDFArticleHandler::ShowResult(FILE *f) {
    for(auto &d : articles_) {
        int i = 0;
        fprintf(f, "==========================\nTime stamp: %ld\n", d.timestamp_);
        fprintf(f, "Title: ");
        for(const auto &s : d.title_) 
            fprintf(f, "%s ", s.c_str());
        fprintf(f, "\n");
        for(std::map<double, std::string>::const_reverse_iterator it = d.tfidf_terms_.rbegin();
                it != d.tfidf_terms_.rend(); ++it) {
            if (++i <= 20)
                fprintf(f, "%s : %lf\n", it->second.c_str(), it->first);
        }
    }
}

