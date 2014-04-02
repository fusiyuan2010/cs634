#include <cstdio>
#include <cmath>

#define MAXV 26

int main(int argc, char *argv[])
{
    double M[MAXV][MAXV];
    double R[MAXV];
    int A[MAXV][MAXV];
    int AT[MAXV][MAXV];
    double a[MAXV], h[MAXV];
    int V = 0;
    int iter;

    scanf("%d %d\n", &V, &iter);
    for(int i = 0; i < V; i++) {
        a[i] = 1;
        h[i] = 1;
        for(int j = 0; j < V; j++) 
            A[i][j] = 0;
    }

    char f, t;
    while(scanf("%c %c\n", &f, &t) != EOF) {
        A[f - 'a'][t - 'a'] = 1;
    }


    /* get transpose */
    for(int i = 0; i < V; i++)
        for(int j = 0; j < V; j++)
            AT[i][j] = A[j][i];

    printf("A:\n");
    for(int i = 0; i < V; i++) {
        for(int j = 0; j < V; j++) {
            printf("%d  ", A[i][j]);
        }
        printf("\n");
    }
    
    printf("AT:\n");
    for(int i = 0; i < V; i++) {
        for(int j = 0; j < V; j++) {
            printf("%d  ", AT[i][j]);
        }
        printf("\n");
    }
 
    while(iter--) {
        double a2[MAXV], h2[MAXV];
        double R2[MAXV];
        printf("Iter %d\n", iter);

        
        for(int i = 0; i < V; i++) {
            a2[i] = 0;
            h2[i] = 0;
            for(int j = 0; j < V; j++) {
                a2[i] += h[j] * AT[i][j];
                h2[i] += a[j] * A[i][j];
            }
        }

        double nora = 0, norh = 0;
        for(int i = 0; i < V; i++) {
            nora += a2[i] * a2[i];
            norh += h2[i] * h2[i];
        }

        nora = sqrt(nora);
        norh = sqrt(norh);

        for(int i = 0; i < V; i++) {
            a[i] = a2[i] / nora;
            h[i] = h2[i] / norh;
        }

        printf("a:  ");
        for(int i = 0; i < V; i++) {
            printf("%.3f ", a[i]);
        }
        printf("\n");

        printf("h:  ");
        for(int i = 0; i < V; i++) {
            printf("%.3f ", h[i]);
        }
        printf("\n");
    }

    return 0;
}


