#include <cstdio>

#define MAXV 26
int main(int argc, char *argv[])
{
    double M[MAXV][MAXV];
    double R[MAXV];
    int E[MAXV][MAXV];
    int N[MAXV];
    int V = 0;
    double c;
    int iter;

    scanf("%d %lf %d\n", &V, &c, &iter);
    for(int i = 0; i < V; i++) {
        N[i] =  0;
        if (i == 0 || i == 5)
            R[i] = 0.5;
        else
            R[i] = 0;
        //R[i] = 1 / (double)V;
        for(int j = 0; j < V; j++) 
            E[i][j] = 0;
    }

    char f, t;
    while(scanf("%c %c\n", &f, &t) != EOF) {
        E[f - 'a'][t - 'a'] = 1;
        N[f - 'a']++;
    }


    double z, k = 1 / (double)V;
    for(int i = 0; i < V; i++)
        for(int j = 0; j < V; j++) {
            if (N[i] == 0)
                z = 1 / (double)V;
            else
                z = 0;

            double m;
            if (E[i][j] == 1)
                m = 1 / (double)N[i];
            else
                m = 0;
            M[i][j] = c * (m + z) + (1 - c) * k; 
        }

    printf("M:\n");
    for(int i = 0; i < V; i++) {
        for(int j = 0; j < V; j++) {
            printf("%.2f  ", E[i][j] == 1? 1 / (double)N[i] : 0);
        }
        printf("\n");
    }

    printf("Z:\n");
    for(int i = 0; i < V; i++) {
        for(int j = 0; j < V; j++) {
            printf("%.2f  ", N[i] == 0? 1 / (double)V : 0);
        }
        printf("\n");
    }

    printf("K:\n");
    for(int i = 0; i < V; i++) {
        for(int j = 0; j < V; j++) {
            printf("%.2f  ", 1 / (double)V);
        }
        printf("\n");
    }



    printf("M*:\n");
    for(int i = 0; i < V; i++) {
        for(int j = 0; j < V; j++) {
            printf("%.2f  ", M[i][j]);
        }
        printf("\n");
    }

    
    while(iter--) {
        double R2[MAXV];
        printf("Iter %d\n", iter);

        for(int i = 0; i < V; i++) {
            R2[i] = 0;
            for(int j = 0; j < V; j++) {
                R2[i] += R[j] * M[j][i];
            }
        }
        for(int i = 0; i < V; i++) {
            R[i] = R2[i];
            printf("%.3f ", R[i]);
        }
        printf("\n");
    }

    double MC[MAXV][MAXV];
    for(int i = 0; i < V; i++)
        for(int j = 0; j < V; j++)
            MC[i][j] = M[i][j];

    iter = 50;
    while(iter--) {
        double MT[MAXV][MAXV];
        for(int i = 0; i < V; i++)
            for(int j = 0; j < V; j++) {
                MT[i][j] = 0;
                for(int k = 0; k < V; k++) {
                    MT[i][j] += M[i][k] * MC[k][j];
                }
            }

        for(int i = 0; i < V; i++)
            for(int j = 0; j < V; j++)
                MC[i][j] = MT[i][j];

    printf("MC %d:\n", iter);
    for(int i = 0; i < V; i++) {
        for(int j = 0; j < V; j++) {
            printf("%.3f  ", MC[i][j]);
        }
        printf("\n");
    }


    }


    return 0;
}


