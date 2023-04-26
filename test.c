#include <stdio.h>
#include <stdlib.h>


void f(int *v)
{
    free(v);
    v = NULL;
}

int main()
{   
    int **v = malloc(3 * sizeof(int *));
    int *a = malloc(sizeof(int));
    *a = 5;
    v[0] = a;
    v[1] = a;
    v[2] = a;

    for (int i = 0; i < 3; ++i)
        printf("%d ", *(v[i]));
    
    printf("\n");
    // free(a);
    *a = NULL;
    for (int i = 0; i < 3; ++i)
        if (v[i])
        printf("%d ", *(v[i]));
    printf("\n");

    // int size = 0, cap = 1;
    // int a = 5;

    // int qux[100] = {1, 6, 8, 20, 101};
    // int ok = 0;
    // int size = 4;
    
    // int v[100] = {2, 21, 19, 101};
    
    // for (int i = 0; i < 4; ++i)
    //     printf("el here at pos %d\n", search(qux, 5, v[i]));

    // for (int i = 0; i < size; ++i)
    //     printf("%d ", qux[i]);
    //     printf("\n");
    // 
   
    
    return 0;
}