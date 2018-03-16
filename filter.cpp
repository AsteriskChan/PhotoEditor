#include "filter.h"
#include <algorithm>

using namespace std;

int partition(int *array, int left, int right)
{
    if (array == NULL)
        return -1;

    int pos = right;
    right--;
    while (left <= right)
    {
        while (left < pos && array[left] <= array[pos])
            left++;

        while (right >= 0 && array[right] > array[pos])
            right--;

        if (left >= right)
            break;

        swap(array[left], array[right]);
    }
    swap(array[left], array[pos]);

    return left;
}

double findMedian (int *array, int size)
{
    if (array == NULL || size <= 0)
        return -1;

    int left = 0;
    int right = size - 1;
    int midPos = right >> 1;
    int index = -1;

    while (index != midPos)
    {
        index = partition(array, left, right);

        if (index < midPos)
        {
            left = index + 1;
        }
        else if (index > midPos)
        {
            right = index - 1;
        }
        else
        {
            break;
        }
    }
    return array[index];
}
