#include "LDA.h"
#include "math.h"

#include "ArticleHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <sys/time.h>

#ifndef COKUS_H
#define COKUS_H
//
// uint32 must be an unsigned integer type capable of holding at least 32
// bits; exactly 32 should be fastest, but 64 is better on an Alpha with
// GCC at -O3 optimization so try your options and see what's best for you
//

typedef unsigned long uint32;

#define ranN              (624)                 // length of state vector
#define ranM              (397)                 // a period parameter
#define ranK              (0x9908B0DFU)         // a magic constant
#define hiBit(u)       ((u) & 0x80000000U)   // mask all but highest   bit of u
#define loBit(u)       ((u) & 0x00000001U)   // mask all but lowest    bit of u
#define loBits(u)      ((u) & 0x7FFFFFFFU)   // mask     the highest   bit of u
#define mixBits(u, v)  (hiBit(u)|loBits(v))  // move hi bit of u to hi bit of v

static uint32   state[ranN+1];     // state vector + 1 extra to not violate ANSI C
static uint32   *next;          // next random value is computed from here
static int      left = -1;      // can *next++ this many times before reloading

static void seedMT(uint32 seed)
{
    register uint32 x = (seed | 1U) & 0xFFFFFFFFU, *s = state;
    register int    j;

    for(left=0, *s++=x, j=ranN; --j;
        *s++ = (x*=69069U) & 0xFFFFFFFFU);
}


static uint32 reloadMT(void)
{
    register uint32 *p0=state, *p2=state+2, *pM=state+ranM, s0, s1;
    register int    j;

    if(left < -1)
        seedMT(4357U);

    left=ranN-1, next=state+1;

    for(s0=state[0], s1=state[1], j=ranN-ranM+1; --j; s0=s1, s1=*p2++)
        *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? ranK : 0U);

    for(pM=state, j=ranM; --j; s0=s1, s1=*p2++)
        *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? ranK : 0U);

    s1=state[0], *p0 = *pM ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? ranK : 0U);
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9D2C5680U;
    s1 ^= (s1 << 15) & 0xEFC60000U;
    return(s1 ^ (s1 >> 18));
 }


static uint32 randomMT(void)
{
    uint32 y;

    if(--left < 0)
        return(reloadMT());

    y  = *next++;
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9D2C5680U;
    y ^= (y << 15) & 0xEFC60000U;
    y ^= (y >> 18);
    return(y);
 }

#endif

#define myrand() (double) (((unsigned long) randomMT()) / 4294967296.)
#define	TRIGGER	1

using namespace std;

int LDA::drawMultinomial(float cdf[], int len)
{
	int result = 0;
	// scaled sample because of unnormalized p[]
	double u = myrand() * cdf[len - 1];
	// do binary search
	int low = 0, high = len - 1;
	while (low <= high)
	{
		result = (high + low) / 2;
		if (u > cdf[result - 1] && u < cdf[result])
			break;
		if (u < cdf[result - 1])
		{
			high = result - 1;
		}
		else
		{
			low = result + 1;
		}
	}

	return result;
}

int LDA::drawMultinomial(int cdf[], int len)
{
	int result = 0;
	// scaled sample because of unnormalized p[]
	double u = myrand() * cdf[len - 1];
	// do binary search
	int low = 0, high = len - 1;
	while (low <= high)
	{
		result = (high + low) / 2;
		if (u > cdf[result - 1] && u < cdf[result])
			break;
		if (u < cdf[result - 1])
		{
			high = result - 1;
		}
		else
		{
			low = result + 1;
		}
	}

	return result;
}




int LDA::init_est()
{
	int m, n, w, k;
	for (int i = 0; i < 500; i++)
		tmpstep[i] = 0;

	p = new float[K];
	c = new int[K];
	// + read training data

	TotalV = 0;
	// K: from command line or default value
	// alpha, beta: from command line or default values
	// niters, savestep: from command line or default values
	nw = new float*[V];
	z = new int**[M];
	for (w = 0; w < V; w++)
	{
		nw[w] = new float[K];
		for (k = 0; k < K; k++)
		{
			nw[w][k] = 0;
		}
	}
	samplestep = new int**[M];
	ndsum = new int[M];
	nd = new float*[M];
	for (m = 0; m < M; m++)
	{
		nd[m] = new float[K];
		ndsum[m] = 0;
		for (k = 0; k < K; k++)
		{
			nd[m][k] = 0;
		}
	}

	nwsum = new float[K];
	for (int k = 0; k < K; k++)
	{
		nwsum[k] = 0;
		c[k] = 0;
	}
	//srandom(time(0)); // initialize for random number generation
	for (m = 0; m < ptrndata->M; m++)
	{
		int N = ptrndata->docs[m]->length; //be careful of this N!!!
		z[m] = new int*[N];
		samplestep[m] = new int *[N];
		// initialize for z
		for (n = 0; n < N; n++)
		{
			int weight = ptrndata->docs[m]->weights[n];
			TotalV+=weight;
			z[m][n] = new int[weight];
			if (weight > TRIGGER -1)
				samplestep[m][n] = new int[weight]; //sampling step
            else
                samplestep[m][n] = nullptr;
			for (int q = 0; q < weight; q++)
			{
				if (weight > TRIGGER -1)
				{
					samplestep[m][n][q] = 0;
				}
				int topic = (int) (myrand() * K);
				// number of instances of word i assigned to topic j
				nw[ptrndata->docs[m]->words[n]][topic] += 1;
				// total number of words assigned to topic j
				nwsum[topic] += 1;
				z[m][n][q] = topic;
				// number of words in document i assigned to topic j
				nd[m][topic] += 1;
			}
			if (weight > TRIGGER -1)
				samplestep[m][n][weight - 1] = gamma; //[0,0,...,0,1] at beginning, the sampling step is small
			// for a unique word, we have to sample N times.
			// the distribution will change during the sampling procedure.
			ndsum[m] += weight;
		}
		// total number of words in document i

	}
	theta = new float*[M];
	for (m = 0; m < M; m++)
	{
		theta[m] = new float[K];
	}

	phi = new float*[K];
	for (k = 0; k < K; k++)
	{
		phi[k] = new float[V];
	}

    pw = new double[V];


	Vbeta = V * beta;
	Kalpha = K * alpha;
	return 0;
}

void LDA::estimate()
{
	printf("|Dynamic sampling\n|Dumping factor: %d\n| %d iterations!\n", gamma,niters);
	gettimeofday(&start, 0);
	int last_iter = liter;
	//	printf("%d,%d",start_weight, initial_stimes);

	for (liter = last_iter + 1; liter <= niters + last_iter; liter++)
	{
		int stimes=0;
		printf("Iteration %d ...", liter);
		for (int m = 0; m < M; m++)
		{
			for (int n = 0; n < ptrndata->docs[m]->length; n++)
			{
				//for each word, sample a step length
				int weight = ptrndata->docs[m]->weights[n];
				int w = ptrndata->docs[m]->words[n];
				if (weight < TRIGGER)
				{ //do normal sampling
					for (int q = 0; q < weight; q++)
					{
						int topic = z[m][n][q];
						nw[w][topic] -= 1;
						nd[m][topic] -= 1;
						nwsum[topic] -= 1;
						p[0] = (nw[w][0] + beta) / (nwsum[0] + Vbeta)
								* (nd[m][0] + alpha);
						// do multinomial sampling via cumulative method

						for (int k = 1; k < K; k++)
						{
							p[k] = p[k - 1] + (nw[w][k] + beta) / (nwsum[k]
									+ Vbeta) * (nd[m][k] + alpha);
						}
						topic = drawMultinomial(p, K);

						// add newly estimated z_i to count variables
						nw[w][topic] += 1;
						nd[m][topic] += 1;
						nwsum[topic] += 1;
						z[m][n][q] = topic;
						stimes++;
					}
				}
				else
				{
					//do dynamic sampling
					tmpstep[0] = samplestep[m][n][0];
					for (int q = 1; q < weight; q++)
					{
						tmpstep[q] = samplestep[m][n][q] + tmpstep[q - 1];
					}
					int stepLen = drawMultinomial(tmpstep, weight) + 1;

					for (int q = 0; q < stepLen; q++)
					{ //do normal sampling
						int topic = z[m][n][q];
						nw[w][topic] -= 1;
						nd[m][topic] -= 1;
						nwsum[topic] -= 1;
						p[0] = (nw[w][0] + beta) / (nwsum[0] + Vbeta)
								* (nd[m][0] + alpha);
						// do multinomial sampling via cumulative method

						for (int k = 1; k < K; k++)
						{
							p[k] = p[k - 1] + (nw[w][k] + beta) / (nwsum[k]
									+ Vbeta) * (nd[m][k] + alpha);
						}
						topic = drawMultinomial(p, K);

						// add newly estimated z_i to count variables
						nw[w][topic] += 1;
						nd[m][topic] += 1;
						nwsum[topic] += 1;
						z[m][n][q] = topic;
						c[topic] = 1;
						stimes++;
					}
					int uniqueNum = 0;
					for (int k = 0; k < K; k++)
					{
						uniqueNum += c[k];
						c[k] = 0;
					}
					samplestep[m][n][uniqueNum] += 1;
				}
			}
		}
		printf("average sampling rate:%.3f\n",stimes/(double)TotalV);
		if (savestep > 0)
		{
			if (liter % savestep == 0)
			{
				gettimeofday(&end, 0);
				timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec
						- start.tv_usec;
				timeuse /= 1000000;

				printf("Running speed: %.3f secs/iteration.\n", timeuse / savestep);
				// saving the model
				printf("Saving the model at iteration %d ...\n", liter);
				compute_theta();
				compute_phi();
	            save_model(liter);
				gettimeofday(&start, 0);
			}
		}

	}
	printf("Gibbs sampling completed!\n");
	printf("Saving the final model!\n");
	compute_theta();
	compute_phi();
	liter--;
	save_model(liter);
}

void LDA::compute_theta()
{
	for (int m = 0; m < M; m++)
	{
		for (int k = 0; k < K; k++)
		{
			theta[m][k] = (nd[m][k] + alpha) / (ndsum[m] + Kalpha);

		}
	}
}

void LDA::compute_phi()
{
	for (int k = 0; k < K; k++)
	{
		for (int w = 0; w < V; w++)
		{
			phi[k][w] = (nw[w][k] + beta) / (nwsum[k] + Vbeta);
		}
	}

    for(int w = 0; w < V; w++) {
        pw[w] = 0;
        for(int k = 0; k < K; k++)
            pw[w] += phi[k][w];
    }
}

void LDA::save_model(int iter) 
{
    char filename[128];
    snprintf(filename, 128, "%s/lda_iter_%d.txt", dir, iter);
    FILE *f = fopen(filename, "w");

    std::multimap<double, int> sorted_fea;
    for(int z = 0; z < K; z++) {
        std::vector<std::pair<int, double>> id_p;
        for (int wi = 0; wi < V; wi ++) 
            id_p.push_back(make_pair(wi, phi[z][wi]));

        sort(id_p.begin(), id_p.end(), 
                [](const std::pair<int, double>& a, const std::pair<int, double>& b){ return a.second > b.second;});

        double total_rate = 0;
        for(size_t i = 0; i < id_p.size() && i < 30; i++) {
            double c = pow(8, phi[z][id_p[i].first] / pw[id_p[i].first]) * 12.5;
            total_rate += c;
        }
        total_rate /= 30;
        //total_rate /= (1 + 50) * 25;

        sorted_fea.insert(make_pair(total_rate, z));
    }

    int rank = 0;
    for(std::multimap<double, int>::reverse_iterator zit = sorted_fea.rbegin();
            zit != sorted_fea.rend(); zit++, rank++) {
        int z = zit->second;
        fprintf(f, "Rank:%d - Topic %2d ( innerw: %f ):\n", rank, z, zit->first);
        std::vector<std::pair<int, double>> id_p;
        for (int wi = 0; wi < V; wi ++) {
            id_p.push_back(make_pair(wi, 
                        phi[z][wi] * (phi[z][wi] / pw[wi])
                        * (phi[z][wi] / pw[wi]) * 100));
        }
        sort(id_p.begin(), id_p.end(), 
                [](const std::pair<int, double>& a, const std::pair<int, double>& b){ return a.second > b.second;});
        int twords = 0;
        for(size_t i = 0; i < id_p.size() && twords < 20; i++) {
            fprintf(f, "\t\t%s:%f ( %.2f%% ) \n",
                    id_term_[id_p[i].first].c_str(), phi[z][id_p[i].first],
                    phi[z][id_p[i].first] * 100 / pw[id_p[i].first]);
            twords++;
            /*
            fprintf(f, "\t\t%s:%f\t\t\t%s:%f\n",
                    id_term_[id_p[i].first].c_str(), id_p[i].second,
                    id_term_[id_p2[i].first].c_str(), id_p2[i].second);
            fprintf(f, "\t\t\t\t");
            for(int k = 0; k < K; k++) {
                fprintf(f, "%.4f  ", phi[k][id_p[i].first]);
            }
            fprintf(f, "\n");
            */
        }
        fprintf(f, "\n");
        id_p.clear();

        fprintf(f, "\tArticles:\n");
        for (int di = 0; di < M; di ++) {
            id_p.push_back(make_pair(di, theta[di][z]));
        }
        sort(id_p.begin(), id_p.end(), 
                [](const std::pair<int, double>& a, const std::pair<int, double>& b){ return a.second > b.second;});
        for(size_t i = 0; i < id_p.size() && i < 40; i++) {
            fprintf(f, "\t\t%s:%f\n", ptrndata->docs[id_p[i].first]->title.c_str(), id_p[i].second);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

LDA::LDA(const char *_dir, double _alpha, double _beta, int ntopics, int _gamma, int iters)
{
    alpha = _alpha;
    beta = _beta;
    K = ntopics;
    gamma = _gamma;
    niters = iters;
    dir = _dir;
    savestep = iters / 10;
    liter = 0;
}

void LDA::Import(const TFIDFArticleHandler *article_handler, double TERMTOP)
{
 	ptrndata = new dataset(article_handler->size());

    int term_id = 0;
    for(size_t i = 0; i < article_handler->size(); i++) {
        const std::multimap<double, std::string>& terms = article_handler->GetArticleTFIDFTerms(i);
        int limit = 0;
        document *doc = new document;

        if (TERMTOP > 1)
            doc->length = std::min((int)TERMTOP, (int)(terms.size()));
        else
            doc->length = terms.size() * TERMTOP;
        doc->words = new int[doc->length];
        doc->weights = new int[doc->length];


        for(std::map<double, std::string>::const_reverse_iterator it = terms.rbegin();
                it != terms.rend() && limit < doc->length; it++, limit++) {
            auto it2 = (article_handler->GetArticleTermFreq(i)).find(it->second);
            int w;
            if (it2 != article_handler->GetArticleTermFreq(i).end()) 
                w = it2->second;
            else
                w = 1;

            if (term_id_.count(it->second) > 0) {
                doc->words[limit] = term_id_[it->second];
                doc->weights[limit] = w;
            } else {
                id_term_[term_id] = it->second;
                term_id_[it->second] = term_id;

                doc->words[limit] = term_id;
                doc->weights[limit] = w;

                term_id++;
            }
        }
        for(const auto & t : article_handler->GetTitle(i))
            doc->title += t + " ";
        ptrndata->add_doc(doc, i);
    }


    ptrndata->V = term_id;

	// + allocate memory and assign values for variables
	M = ptrndata->M;
	V = ptrndata->V;
}

void LDA::Compute()
{
    init_est();
    estimate();
}

void LDA::Finish()
{
    delete[] p;
    delete[] c;

    for(int w = 0; w < V; w++)
        delete[] nw[w];

    delete[] nw;
    delete[] nwsum;

    for (int m = 0; m < ptrndata->M; m++) {
        int N = ptrndata->docs[m]->length; //be careful of this N!!!
        delete[] nd[m];
        for (int n = 0; n < N; n++) {
            delete[] z[m][n];
            delete[] samplestep[m][n];
        }
        delete[] z[m];
        delete[] samplestep[m];
        delete[] theta[m];
    }

    for(int k = 0; k < K; k++)
        delete[] phi[k];

    delete[] phi;
    delete[] theta;
    delete[] samplestep;

    delete[] nd;
    delete[] z;
    delete[] pw;
}
