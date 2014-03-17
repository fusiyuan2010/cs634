#ifndef _PLSA_H_
#define  _PLSA_H_

#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdio>

class TFIDFArticleHandler;

class PLSA {
    double *p_dz, *p_wz, *p_z;
    std::vector<std::vector<std::pair<int, int>>> docs_;
    std::unordered_map<int, std::string> id_term_;
    std::unordered_map<std::string, int> term_id_;
    std::unordered_map<int, std::string> id_title_;

    void init();
    double likelihood();
    void update();
    void train();

    int d;
    int w;
    int iteration_min;
    int iteration_max;
    int z;
    double tol;
public:
    PLSA(int _feature_num, int _iteration_min,
            int _iteration_max, double _tol = 0.001)
        : iteration_min(_iteration_min),
        iteration_max(_iteration_max),
        z(_feature_num),
        tol(_tol) {}
    void Import(const TFIDFArticleHandler *article_handler);
    void Compute();
    void OutputRaw(FILE *f);
    void Finish();
};


#endif

