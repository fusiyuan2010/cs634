#include "Parser.h"
#include <cstring>
#include <ctype.h>
#include <time.h> 

#define prefixcmp(a, b) strncmp(a, b, strlen(b))

void Parser::ParseLine(const char *str) {
    const char *s = NULL;
    
#ifdef NG
    if (prefixcmp(str, "From: ") == 0) {
        cur_article_.clear();
    } else if (prefixcmp(str, "Subject: ") == 0 ||
            prefixcmp(str, "Organization: ") == 0 ||
            prefixcmp(str, "Lines: ") == 0 ||
            prefixcmp(str, "Nntp-Posting-Host: ") == 0 ||
            prefixcmp(str, "X-Newsreader: ") == 0 || 
            prefixcmp(str, "In article ") == 0) {
    } else if (prefixcmp(str, "===EOF===") == 0) {
        if (!cur_article_.empty())
            article_handler_->AddArticle(cur_title_, cur_article_, cur_timestamp_);
        cur_article_.clear();
    } else {
        s = str;
        if (str[0] == '>')
            s++;
            add_content(s);
    } 
#endif
#ifdef REUTERS
    if (prefixcmp(str, "<DATE>") == 0) {
        char date_str[32] = {0};
        s = str + strlen("<DATE>");
        int i;
        for(i = 0; s[i] != '<' && i < 22; i++) {
            date_str[i] = s[i];
        }
        date_str[i] = '\0';
        struct tm tm;
        strptime(date_str, "%d-%b-%Y %H:%M:%S.", &tm);
        cur_timestamp_ = mktime(&tm);
    } else if (prefixcmp(str, "<REUTERS") == 0) {
    } else if (prefixcmp(str, "<TEXT>") == 0) {
    } else if (prefixcmp(str, "<TITLE>") == 0) {
        s = str + strlen("<TITLE>");
        add_title(s);
    } else if ((s = strstr(str, "<BODY>")) != NULL) {
        s = s + strlen("<BODY>");
        state_ = S_IN_CONTENT;
        add_content(s);
    } else if (prefixcmp(str, "</REUTERS>") == 0) {
        article_handler_->AddArticle(cur_title_, cur_article_, cur_timestamp_);
        state_ = S_NONE;
        cur_title_.clear();
        cur_article_.clear();
    } else if (state_ == S_IN_CONTENT) {
        add_content(str);
    }
#endif
#ifdef IMDB
    if (prefixcmp(str, "MV:") == 0) {
        state_ = S_IN_CONTENT;
    } else if (prefixcmp(str, "PL:") == 0 && state_ == S_IN_CONTENT) {
        s = str + 4;
        add_content(s);
    } else if (prefixcmp(str, "BY:") == 0) {
        article_handler_->AddArticle(cur_title_, cur_article_, cur_timestamp_);
        state_ = S_NONE;
        cur_title_.clear();
        cur_article_.clear();
    }
#endif
}


void Parser::add_title(const char *str) {
    char word[32];
    std::string word2;
    int p = 0;
    for(int i = 0; str[i] != '\n' && str[i] != '\0' && prefixcmp(str + i, "</TITLE>"); i++) {
        if (isblank(str[i]) || p == 31) {
            word[p] = '\0';
            p = 0;
            if (!(word2 = word_transformer_->Transform(word)).empty())
                cur_title_.emplace_back(word2);
        } else
            word[p++] = str[i];
    }
    word[p] = '\0';
    if (!(word2 = word_transformer_->Transform(word)).empty())
        cur_title_.emplace_back(word2);
}

void Parser::add_content(const char *str) {
    char word[32];
    std::string word2;
    int p = 0;
    for(int i = 0; str[i] != '\n' && str[i] != '\0'; i++) {
        if (isblank(str[i]) || p == 32) {
            word[p] = '\0';
            p = 0;
            if (!(word2 = word_transformer_->Transform(word)).empty())
                cur_article_.emplace_back(word2);
        } else
            word[p++] = str[i];
    }
    word[p] = '\0';
    if (!(word2 = word_transformer_->Transform(word)).empty())
        cur_article_.emplace_back(word2);
}
#undef prefixcmp

