/* this file are mainly copied from https://github.com/thinxer/plsi */
#include "PLSA.h"
#include "ArticleHandler.h"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>
#include <chrono>
#include <random>
#include <algorithm>

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
    for (int iteration = 0; iteration < iteration_max; iteration++) {
        update();
        // debug();
        double now_likelihood = likelihood();
        fprintf(stdout, "iteration: %d, log-likelihood: %f\n", iteration, now_likelihood);
        if (fabs(now_likelihood - last_likelihood) < tol 
                && iteration >= iteration_min) break;
        last_likelihood = now_likelihood;
        if (iteration % 100 == 0) {
            //output();
        }
    }
}

void PLSA::Finish() {
    delete[] p_dz;
    delete[] p_wz;
    delete[] p_z;
    docs_.clear();
    id_term_.clear();
    term_id_.clear();
}

void PLSA::Import(const TFIDFArticleHandler *article_handler)
{
    int term_id = 0;
    for(size_t i = 0; i < article_handler->size(); i++) {
        const std::map<double, std::string>& terms = article_handler->GetArticleTFIDFTerms(i);
        int limit = 0;
        vector<pair<int, int>> doc; 
        for(std::map<double, std::string>::const_reverse_iterator it = terms.rbegin();
                it != terms.rend() && ++limit != 50; it++) {
            int w = it->first * 2;
            if (w == 0) 
                w = 1;
            if (term_id_.count(it->second) > 0) 
                doc.push_back(make_pair(term_id_[it->second], w));
            else {
                id_term_[term_id] = it->second;
                term_id_[it->second] = term_id;
                doc.push_back(make_pair(term_id, w));
                term_id++;
            }
        }
        for(const auto & t : article_handler->GetTitle(i))
            id_title_[i] += t + " ";
        docs_.push_back(doc);
    }
    d = docs_.size();
    w = term_id_.size();
    iteration_max = 100000;
}

void PLSA::Compute()
{
    init();
    train();
}

void PLSA::OutputRaw() {
    FILE* file_dz = fopen("plsa_result.txt", "w");
    for (int zi = 0; zi < z; zi ++) {
        fprintf(file_dz, "feature z%02d = %f\n", zi, p_z[zi]);

        std::vector<std::pair<int, double>> id_p;
        for (int wi = 0; wi < w; wi ++) {
            id_p.push_back(make_pair(wi, p_wz[wi * z + zi]));
        }
        sort(id_p.begin(), id_p.end(), 
                [](const std::pair<int, double>& a, const std::pair<int, double>& b){ return a.second > b.second;});
        for(size_t i = 0; i < id_p.size() && i < 20; i++) {
            fprintf(file_dz, "\t\t%s:%.2f\n", id_term_[id_p[i].first].c_str(), id_p[i].second);
        }
        fprintf(file_dz, "\n");
        id_p.clear();

        fprintf(file_dz, "\tArticles:\n");
        for (int di = 0; di < d; di ++) {
            id_p.push_back(make_pair(di, p_dz[di * z + zi]));
        }
        sort(id_p.begin(), id_p.end(), 
                [](const std::pair<int, double>& a, const std::pair<int, double>& b){ return a.second > b.second;});
        for(size_t i = 0; i < id_p.size() && i < 40; i++) {
            fprintf(file_dz, "\t\t%s:%f\n", id_title_[id_p[i].first].c_str(), id_p[i].second);
        }
        fprintf(file_dz, "\n");

        /*
        for (int di = 0; di < d; di ++) {
            if (di > 0) fprintf(file_dz, " ");
            fprintf(file_dz, "%f", p_dz[di * z + zi]);
        }
        fprintf(file_dz, "\n");
        */
    }
    fclose(file_dz);
}

