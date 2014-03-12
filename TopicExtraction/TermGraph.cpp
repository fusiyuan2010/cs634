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
            if (last != empty && last.weight_ > 8)
                pair2.push_back(last);
            last = p;
        } else {
            last.weight_++;
        }
    if (last.weight_ > 8)
        pair2.push_back(last);
    pairs_ = std::move(pair2);
}

void TermGraph::ShowResult() {
    for(const auto& p : pairs_) 
        printf("%s - %s : %d\n", p.t1_.c_str(), p.t2_.c_str(), p.weight_);
}


void TermGraph::Import(const TFIDFArticleHandler *article_handler) {
    for(size_t i = 0; i < article_handler->size(); i++) {
        const std::map<double, std::string>& terms = article_handler->GetArticleTFIDFTerms(i);
        std::vector<std::string> terms_list;
        int limit = 0;
        for(std::map<double, std::string>::const_reverse_iterator it1 = terms.rbegin();
                it1 != terms.rend() && ++limit != 12; it1++) {
            int limit2 = limit;
            for(std::map<double, std::string>::const_reverse_iterator it2 = it1;
                    it2 != terms.rend() && ++limit2 != 12; it2++) {
                if (it2->second == it1->second)
                    continue;
                int weight = std::max(it1->first * it2->first, (double)2);
                Add({it1->second, it2->second, weight});
                Add({it2->second, it1->second, weight});
            }
        }
    }
}

