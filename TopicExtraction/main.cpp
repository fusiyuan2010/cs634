#include <cstdio>
#include <cstring>

#include "Parser.h"
#include "WordTransformer.h"
#include "ArticleHandler.h"
#include "TermGraph.h"
//#include "PLSA.h"

int main(int argc, char *argv[])
{
    char buf[1024];
    SimpleWordTransformer word_transformer;
    TFIDFArticleHandler article_handler;

    word_transformer.ReadBlacklist(argv[1]);
    Parser parser(&article_handler, &word_transformer);
    while(fgets(buf, 1024, stdin) != NULL) {
        size_t i = strlen(buf);
        if (i > 0) {
            buf[i - 1] = '\0';
            parser.ParseLine(buf);
        }
    }
    article_handler.GetTFIDF();
    //article_handler.ShowResult();


    TermGraph term_graph;
    term_graph.Import(&article_handler);
    term_graph.SortAndReduce();
    term_graph.ShowResult();

    return 0;
}


