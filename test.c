#include <stdio.h>
#include <stdlib.h>

int s_b(int *v, int size, int target)
{
    int left = 0, right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (v[mid] == target)
            return mid;
        else if (v[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return left;
}

int s_c(int *v, int size, int target)
{
    for (int i = 0; i < size; ++i) {
        if (target < v[i])
            return i;
        else if (target == v[i])
            return i;

    }
    return size;
}

int main()
{
    int v[100] = {1, 2, 5, 9, 20, 22};
    int s[100] = {0, 5, 6, 8, 10, 21, 25};

    for (int i = 0; i < 7; ++i) {
        int pos1 = s_b(v, 6, s[i]);
        int pos2 = s_c(v, 6, s[i]);

        printf("correct = %d, binarry = %d\n", pos2, pos1);
    }

    return 0;
}