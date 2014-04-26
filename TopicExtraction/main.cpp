#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <getopt.h>

#include "Parser.h"
#include "WordTransformer.h"
#include "ArticleHandler.h"
#include "TermGraph.h"
#include "PLSA.h"
#include "LDA.h"

void print_usage(int argc, char *argv[])
{
    printf("Usage: %s [opts]\n"
           "\t--dir <path> [default: ./output/],  result output path\n"
           "\t--function_words <filename> [default: null],  filename of function word list\n"
           "\t--parser <ap|reuters|newsgroup|imdb> [default: ap],  use what kind of parser\n"
           "\t--plsa <1|0> [default: 0],  do plsa computing\n"
           "\t--plsa_threshold <float_number> [default: 10],  stop condition for iteration\n"
           "\t--iter_min <int_number> [default: 100],  min iteration times\n"
           "\t--iter_max <int_number> [default: 10000],  max iteration times\n"
           "\t--topic_num <int_number> [default: 30],  latent topics assumed\n"
           "\t--term_graph <1|0> [default: 0],  do term similarity computing\n"
           "\t--lda <1|0> [default: 0],  do LDA(ecgs) computing\n"
           "\t--tfidf_fomula <1|2> [default: 1],  use what kind of tfidf_fomula\n"
           "\t--term_top <float_number >[default 10] select N top tfidf result as\n\t\t"
           "LDA input(N = term_top if > 1, term_top * size() if < 1\n", 
           argv[0]);
}

int main(int argc, char *argv[])
{

    const char *dir = "./output/";
    const char *parser_name = "ap";
    const char *function_words = nullptr;
    int iter_min = 100;
    int iter_max = 10000;
    double plsa_threshold = 10;
    int do_plsa = 0;
    int do_lda = 0;
    int do_term_graph = 0;
    int tfidf_fomula = 1;
    int topic_num = 30;
    double term_top = 10;
    

    static struct option long_options[] = {
            {"dir",     required_argument, 0,  'a' },
            {"iter_max",  required_argument,       0,  'b' },
            {"iter_min",  required_argument, 0,  'c' },
            {"plsa_threshold",  required_argument, 0,  'd' },
            {"plsa",  required_argument, 0,  'e' },
            {"term_graph",  required_argument, 0,  'f' },
            {"tfidf_fomula",  required_argument, 0,  'g' },
            {"parser",  required_argument, 0,  'h' },
            {"function_words",  required_argument, 0,  'i' },
            {"topic_num",  required_argument, 0,  'j' },
            {"lda",  required_argument, 0,  'k' },
            {"term_top",  required_argument, 0,  'l' },
            {0,         0,                 0,  0 }
        };

    int long_index = 0;
    int opt;
    while((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1) {
        switch(opt) {
            case 'a':
                dir = optarg;
                break;
            case 'b':
                iter_max = atoi(optarg);
                break;
            case 'c':
                iter_min = atoi(optarg);
                break;
            case 'd':
                plsa_threshold = atof(optarg);
                break;
            case 'e':
                do_plsa = atoi(optarg);
                break;
            case 'f':
                do_term_graph = atoi(optarg);
                break;
            case 'g':
                tfidf_fomula = atoi(optarg);
                break;
            case 'h':
                parser_name = optarg;
                break;
            case 'i':
                function_words = optarg;
                break;
            case 'j':
                topic_num = atoi(optarg);
                break;
            case 'k':
                do_lda = atoi(optarg);
                break;
            case 'l':
                term_top = atof(optarg);
                break;
            default:
                print_usage(argc, argv);
                return 1;
                break;
        }
    }
    srand(time(NULL));

    FILE *f = nullptr;
    char filename[128];
    snprintf(filename, 128, "%s/arguments.txt", dir);
    f = fopen(filename, "w");
    if (f == nullptr) {
        printf("file %s open failed!\n", filename);
        return 1;
    }
    fprintf(f, "plsa = %d\n", do_plsa);
    fprintf(f, "topic_num = %d\n", topic_num);
    fprintf(f, "plsa_threshold = %f\n", plsa_threshold);
    fprintf(f, "iter_max = %d\n", iter_max);
    fprintf(f, "iter_min = %d\n", iter_min);
    fprintf(f, "tfidf_fomula = %d\n", tfidf_fomula);
    fprintf(f, "term_top = %.3f\n", term_top);
    fclose(f);


    char buf[1024];
    SimpleWordTransformer word_transformer;
    TFIDFArticleHandler article_handler(tfidf_fomula);

    if (function_words != nullptr)
        word_transformer.ReadBlacklist(function_words);

    Parser parser(parser_name, &article_handler, &word_transformer);
    while(fgets(buf, 1024, stdin) != NULL) {
        size_t i = strlen(buf);
        if (i > 0) {
            buf[i - 1] = '\0';
            parser.ParseLine(buf);
        }
    }

    snprintf(filename, 128, "%s/tfidf.txt", dir);
    f = fopen(filename, "w");
    article_handler.GetTFIDF();
    article_handler.ShowResult(f);
    fclose(f);

    if (do_term_graph) {
        snprintf(filename, 128, "%s/term_graph.txt", dir);
        f = fopen(filename, "w");

        TermGraph term_graph;
        term_graph.Import(&article_handler);
        term_graph.SortAndReduce();
        term_graph.ShowResult(f);

        fclose(f);
    }
    
    if (do_plsa) {
        snprintf(filename, 128, "%s/plsa_result.txt", dir);
        f = fopen(filename, "w");

        PLSA plsa(topic_num, iter_min, iter_max, plsa_threshold);
        plsa.Import(&article_handler);
        plsa.Compute();
        plsa.OutputRaw(f);
        plsa.Finish();

        fclose(f);
    }

    if (do_lda) {
        LDA lda(&article_handler, dir, (double)50 / topic_num, 0.1, topic_num, 1, iter_max);
        lda.Import(term_top);
        lda.Compute();
        lda.Finish();
    }

    printf("Done\n");

    return 0;
}


