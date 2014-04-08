#ifndef _LDA_H_
#define _LDA_H_

#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdio>

class TFIDFArticleHandler;

class document {
public:
    int * words;
    int * weights;
    std::string title;
    int length;
    int topicid;

     ~document() {
        if (words) {
            delete words;
        }
        if (weights) {
            delete weights;
        }
    }
};


class dataset {
public:
    document ** docs;
    int M; // number of documents
    int V; // number of words
    
    dataset() {
	docs = NULL;
	M = 0;
	V = 0;
    }
    
    dataset(int M) {
	this->M = M;
	this->V = 0;
	docs = new document*[M];	
    }   
    
    ~dataset() {
	if (docs) {
	    for (int i = 0; i < M; i++) {
		delete docs[i];
	    }
	}
	delete docs;
    }
    
    void add_doc(document * doc, int idx) {
        if (0 <= idx && idx < M) {
            docs[idx] = doc;
        }
    }   
};


class LDA {

    std::unordered_map<int, std::string> id_term_;
    std::unordered_map<std::string, int> term_id_;
    std::unordered_map<int, std::string> id_title_;

    dataset * ptrndata;	// pointer to training dataset object
    const char *dir;

    int M; // dataset size (i.e., number of docs)
    int V; // vocabulary size
    int K; // number of topics
    int TotalV; //total number of words
    float alpha, beta, Kalpha, Vbeta; // LDA hyperparameters
    int niters; // number of Gibbs sampling iterations
    int liter; // the iteration at which the model was saved
    int savestep; // saving period
    int twords; // print out top words per each topic
    float * p; // temp variable for sampling
    int * c; //temp variable for counting unique topics
    float ** nw; // cwt[i][j]: number of instances of word/term i assigned to topic j, size V x K
    int *** samplestep; //D*W*Weight
    int *** z;
    int tmpstep[500];
    float ** nd; // na[i][j]: number of words in document i assigned to topic j, size M x K
    float * nwsum; // nwsum[j]: total number of words assigned to topic j, size K
    int * ndsum; // nasum[i]: total number of words in document i, size M
    float ** theta; // theta: document-topic distributions, size M x K
    float ** phi; // phi: topic-word distributions, size K x V
    struct timeval start,end;
    float timeuse;
    int gamma;

    int drawMultinomial(float cdf[], int len);
    int drawMultinomial(int cdf[], int len);
    // init for estimation
    int init_est();
	
    // estimate LDA model using Gibbs sampling
    void estimate();
//    int sampling(int m, int n);
    void compute_theta();
    void compute_phi();
//    float compute_perplexity();
//    void foldin(int m);
    void save_model(int iter);

    //Added By SiyuanFu
    /* weight on each topic per word*/
    double *pw;

public:
    LDA(const char *_dir, double _alpha, double _beta, int ntopics, int _gamma, int iters);

    void Import(const TFIDFArticleHandler *article_handler, double TERMTOP);
    void Compute();
    void Finish();

};

#endif

