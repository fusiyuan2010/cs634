/* this file are mainly copied from https://github.com/thinxer/plsi */
#include "PLSA.h"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>
#include <chrono>
#include <random>

using namespace std;

void PLSA::init() 
{
    p_dz = new double[d * z];
    p_wz = new double[w * z];
    p_z = new double[z];

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int zi = 0; zi < z; zi ++) {
        p_z[zi] = distribution(generator);

        for (int di = 0; di < d; di ++) {
            p_dz[di * z + zi] = distribution(generator);
        }

        for (int wi = 0; wi < w; wi ++) {
            p_wz[wi * z + zi] = distribution(generator);
        }
    }
}

double PLSA::likelihood() 
{
    double r = 0;
    for (int di = 0; di < d; di ++) {
        for (auto& word_tf : docs_[di]) {
            int wi = word_tf.first,
                tf = word_tf.second;
            double p_dw = 0;
            for (int zi = 0; zi < z; zi ++) {
                p_dw += p_z[zi] * p_wz[wi * z + zi] * p_dz[di * z + zi];
            }
            r += tf * log(p_dw);
        }
    }
    return r;
}

void PLSA::update() {
    double *nominator_p_dz = new double[d * z];
    double *denominator_p_dz = new double[z];
    memset(nominator_p_dz, 0, sizeof(double) * d * z);
    memset(denominator_p_dz, 0, sizeof(double) * z);

    double *nominator_p_wz = new double[w * z];
    double *denominator_p_wz = new double[z];
    memset(nominator_p_wz, 0, sizeof(double) * w * z);
    memset(denominator_p_wz, 0, sizeof(double) * z);

    double *nominator_p_z = new double[z];
    double denominator_p_z = 0;
    memset(nominator_p_z, 0, sizeof(double) * z);

    for (int di = 0; di < d; di ++) {
        for (auto& word_tf : docs_[di]) {
            int wi = word_tf.first,
                tf = word_tf.second;
            double denominator = 0;
            double* nominator = new double[z];

            for (int zi = 0; zi < z; zi ++) {
                nominator[zi] = p_dz[di * z + zi] * p_wz[wi * z + zi] * p_z[zi];
                assert(nominator[zi] >= 0);
                denominator += nominator[zi];
            }
            assert(denominator >= 0);

            for (int zi = 0; zi < z; zi ++) {
                double p_z_condition_d_w = nominator[zi] / denominator;

                nominator_p_dz[di * z + zi] += tf * p_z_condition_d_w;
                denominator_p_dz[zi] += tf * p_z_condition_d_w;

                nominator_p_wz[wi * z + zi] += tf * p_z_condition_d_w;
                denominator_p_wz[zi] += tf * p_z_condition_d_w;

                nominator_p_z[zi] += tf * p_z_condition_d_w;
            }
            denominator_p_z += tf;

            delete[] nominator;
        }
    }

    for (int di = 0; di < d; di ++) {
        for (int zi = 0; zi < z; zi ++) {
            p_dz[di * z + zi] = nominator_p_dz[di * z + zi] / denominator_p_dz[zi];
            assert(p_dz[di * z + zi] <= 1);
            assert(p_dz[di * z + zi] >= 0);
        }
    }

    for (int wi = 0; wi < w; wi ++) {
        for (int zi = 0; zi < z; zi ++) {
            p_wz[wi * z + zi] = nominator_p_wz[wi * z + zi] / denominator_p_wz[zi];
            assert(p_wz[wi * z + zi] >= 0);
            assert(p_wz[wi * z + zi] <= 1);
        }
    }

    for (int zi = 0; zi < z; zi ++) {
        p_z[zi] = nominator_p_z[zi] / denominator_p_z;
        assert(p_z[zi] >= 0);
        assert(p_z[zi] <= 1);
    }

    delete[] nominator_p_dz;
    delete[] denominator_p_dz;

    delete[] nominator_p_wz;
    delete[] denominator_p_wz;

    delete[] nominator_p_z;
}


void PLSA::train() {
    double last_likelihood = -1;
    for (iteration = 0; iteration < iteration_max; iteration++) {
        update();
        // debug();
        double now_likelihood = likelihood();
        fprintf(stdout, "iteration: %d, log-likelihood: %f\n", iteration, now_likelihood);
        if (fabs(now_likelihood - last_likelihood) < tol) break;
        last_likelihood = now_likelihood;
        if (iteration % 100 == 0) {
            //output();
        }
    }
}

void PLSA::finish() {
    delete[] p_dz;
    delete[] p_wz;
    delete[] p_z;
}


/*
void output() {
    FILE* file_z = fopen("z", "w");
    for (int zi = 0; zi < z; zi ++) {
        fprintf(file_z, "%f\n", p_z[zi]);
    }
    fclose(file_z);

    FILE* file_wz = fopen("wz", "w");
    for (int zi = 0; zi < z; zi ++) {
        for (int wi = 0; wi < w; wi ++) {
            if (wi > 0) fprintf(file_wz, " ");
            fprintf(file_wz, "%f", p_wz[wi * z + zi]);
        }
        fprintf(file_wz, "\n");
    }
    fclose(file_wz);

    FILE* file_dz = fopen("dz", "w");
    for (int zi = 0; zi < z; zi ++) {
        for (int di = 0; di < w; di ++) {
            if (di > 0) fprintf(file_dz, " ");
            fprintf(file_dz, "%f", p_dz[di * z + zi]);
        }
        fprintf(file_dz, "\n");
    }
    fclose(file_dz);
}
*/

