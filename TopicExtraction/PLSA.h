#ifndef _PLSA_H_
#define  _PLSA_H_

#include <utility>
#include <vector>
#include <string>
#include <unordered_map>

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
    int iteration_max;
    int iteration_min;
    int z;
    constexpr static const double tol = 0.001;
public:
    PLSA(int _feature_num = 50, int _iteration_min = 2000)
        : iteration_min(_iteration_min),z(_feature_num) {}
    void Import(const TFIDFArticleHandler *article_handler);
    void Compute();
    void OutputRaw();
    void Finish();
};


#endif

