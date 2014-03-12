#ifndef _TERMGRAPH_H_
#define _TERMGRAPH_H_
#include <string>
#include <vector>

class TFIDFArticleHandler;

class TermGraph 
{
    class Pair
    {
    public:
        std::string t1_;
        std::string t2_;
        int weight_;

        Pair(const std::string& t1, const std::string &t2, int w = 1)
            : t1_(t1), t2_(t2), weight_(w) {}

        bool operator<(const Pair& p) const {
            return (t1_ < p.t1_) || (t1_ == p.t1_ && t2_ < p.t2_);
        }

        bool operator==(const Pair& p) const {
            return (t1_ == p.t1_ && t2_ == p.t2_);
        }

        bool operator!=(const Pair& p) const {
            return (t1_ != p.t1_ || t2_ != p.t2_);
        }
    };

    std::vector<Pair> pairs_;
public:
    void Clear();
    void Add(const Pair& p);
    void Import(const TFIDFArticleHandler *article_handler);
    void SortAndReduce();
    void ShowResult();
};

#endif
