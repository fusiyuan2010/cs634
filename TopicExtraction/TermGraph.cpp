#include "TermGraph.h"
#include "ArticleHandler.h"
#include <algorithm>
#include <cstdio>

void TermGraph::Clear() {
    pairs_.clear();
}
void TermGraph::Add(const Pair& p) {
    pairs_.push_back(p);
}

void TermGraph::SortAndReduce() {
    sort(pairs_.begin(), pairs_.end());
    std::vector<Pair> pair2;
    Pair last{"", "", 0}, empty = last;

    for(const auto& p : pairs_) 
        if (p != last) {
            if (last != empty && last.weight_ > 2) {
                last.weight_ = last.weight_ * last.weight_ * 10000
                    / (freq_[last.t1_] * freq_[last.t2_]);
                if (last.weight_ > 1)
                    pair2.push_back(last);
            }
            last = p;
        } else {
            last.weight_ += p.weight_;
        }
    if (last.weight_ > 2) {
        last.weight_ = last.weight_ * last.weight_ * 10000
            / (freq_[last.t1_] * freq_[last.t2_]);
        if (last.weight_ > 1)
            pair2.push_back(last);
    }
    pairs_ = std::move(pair2);
}

void TermGraph::ShowResult(FILE *f) {
    for(const auto& p : pairs_) 
        fprintf(f, "%s(%d) - %s(%d) : %d\n", p.t1_.c_str(), freq_[p.t1_],
                p.t2_.c_str(), freq_[p.t2_], p.weight_);
}


void TermGraph::Import(const TFIDFArticleHandler *article_handler) {
    for(size_t i = 0; i < article_handler->size(); i++) {
        const std::multimap<double, std::string>& terms = article_handler->GetArticleTFIDFTerms(i);
        std::vector<std::string> terms_list;
        int limit = 0;
        for(std::map<double, std::string>::const_reverse_iterator it1 = terms.rbegin();
                it1 != terms.rend() && ++limit != 30; it1++) {
            int limit2 = limit;
            for(std::map<double, std::string>::const_reverse_iterator it2 = it1;
                    it2 != terms.rend() && ++limit2 != 30; it2++) {
                if (it2->second == it1->second)
                    continue;
                //int weight = it1->first * it2->first;
                int weight = 1;
                Add({it1->second, it2->second, weight});
                Add({it2->second, it1->second, weight});
            }
            //freq_[it1->second] += it1->first * it1->first;
            freq_[it1->second]++;
        }
    }
}

