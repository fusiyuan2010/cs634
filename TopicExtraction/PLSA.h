#ifndef _PLSA_H_
#define  _PLSA_H_

#include <utility>
#include <vector>

class PLSA {
    double *p_dz, *p_wz, *p_z;
    std::vector<std::vector<std::pair<int, int>>> docs_;

    void init();
    double likelihood();
    void update();
    void train();
    void finish();

    int d;
    int w;
    int iteration;
    int iteration_max;
    static const int z = 50;
    constexpr static const double tol = 0.001;
};


#endif

