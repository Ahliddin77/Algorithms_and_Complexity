#include <algorithm>
#include <chrono>
#include <cassert>
#include <iostream>
#include <random>
using namespace std;

void insertionSort(int* arr, int start, int end) {
    for (int i = start + 1; i < end + 1; i++) {
        int val = arr[i];
        int cur = i;
        while ((cur > 0) && arr[cur - 1] > val) {
            arr[cur] = arr[cur - 1];
            cur--;
        }
        arr[cur] = val;
    }
}

int partition(int arr[], int start, int end)
{
    int pivot = arr[end];
    int i = start - 1;
    for (int j = start; j < end; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[end]);
    return i + 1;
}

void quickSort(int arr[], int start, int end)
{
    if (start >= end)
        return;
    int p = partition(arr, start, end);
    quickSort(arr, start, p - 1);
    quickSort(arr, p + 1, end);
}

void merge(int array[], int l, int m, int r)
{
    const int leftArrSize = m - l + 1;
    const int rightArrSize = r - m;

    int* leftArray = new int[leftArrSize];
    int* rightArray = new int[rightArrSize];

    for (int i = 0; i < leftArrSize; i++)
        leftArray[i] = array[l + i];
    for (int j = 0; j < rightArrSize; j++)
        rightArray[j] = array[m + 1 + j];

    int indexLeftArray = 0;
    int indexRightArray = 0;
    int indexMergedArray = l;

    while (indexLeftArray < leftArrSize && indexRightArray < rightArrSize) {
        if (leftArray[indexLeftArray] <= rightArray[indexRightArray]) {
            array[indexMergedArray] = leftArray[indexLeftArray];
            indexLeftArray++;
        }
        else {
            array[indexMergedArray] = rightArray[indexRightArray];
            indexRightArray++;
        }
        indexMergedArray++;
    }

    while (indexLeftArray < leftArrSize) {
        array[indexMergedArray] = leftArray[indexLeftArray];
        indexLeftArray++;
        indexMergedArray++;
    }

    while (indexRightArray < rightArrSize) {
        array[indexMergedArray] = rightArray[indexRightArray];
        indexRightArray++;
        indexMergedArray++;
    }
    delete[] leftArray;
    delete[] rightArray;
}

void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void maxHeapify(int arr[], int N, int i)
{
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < N && arr[l] > arr[largest])
        largest = l;
    if (r < N && arr[r] > arr[largest])
        largest = r;
    if (largest != i)
    {
        swap(arr[i], arr[largest]);
        maxHeapify(arr, N, largest);
    }
}

void heapSort(int arr[], int N)
{
    for (int i = N / 2 - 1; i >= 0; i--)
    {
        maxHeapify(arr, N, i);
    }
    for (int i = N - 1; i > 0; i--)
    {
        swap(arr[0], arr[i]);
        maxHeapify(arr, i, 0);
    }
}

void hybridSort(int arr[], int start, int end)
{
    if (end - start + 1 < 250)
        insertionSort(arr, start, end);
    else
        while (start < end)
        {
            int pivot = partition(arr, start, end);
            if (pivot - start < end - pivot)
            {
                hybridSort(arr, start, pivot - 1);
                start = pivot + 1;
            }
            else
            {
                hybridSort(arr, pivot + 1, end);
                end = pivot - 1;
            }
        }
}

constexpr int step = 100;
constexpr int maxlen = 10000;
constexpr int times = 100;

using std::chrono::nanoseconds;

nanoseconds timeit(int* original, int* arr, int len, void what(int*, int)) {
    std::copy(original, original + len, arr);

    auto begin = std::chrono::steady_clock::now();
    what(arr, len);
    auto end = std::chrono::steady_clock::now();

    assert(std::is_sorted(arr, arr + len));

    nanoseconds total(end - begin);
    return total;
}

nanoseconds timeit2(int* original, int* arr, int start, int end, void what(int*, int, int)) {
    int len = end - start + 1;
    std::copy(original, original + len, arr);

    auto begin2 = std::chrono::steady_clock::now();
    what(arr, start, end);
    auto end2 = std::chrono::steady_clock::now();

    assert(std::is_sorted(arr, arr + len));

    nanoseconds total(end2 - begin2);
    return total;
}


int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int len = step; len < maxlen; len += step) {
        nanoseconds ins(0);
        nanoseconds quick(0);
        nanoseconds merge(0);
        nanoseconds heap(0);
        nanoseconds hybrid(0);

        int* original = new int[len];
        for (int i = 0; i < len; i++) {
            original[i] = i;
        }

        int* arr = new int[len];
        int* sorted = new int[len];

        for (int t = 0; t < times; t++) {
            std::shuffle(original, original + len, gen);

            ins += timeit2(original, arr, 0, len-1, &insertionSort);
            quick += timeit2(original, arr, 0, len - 1, &quickSort);
            merge += timeit2(original, arr, 0, len - 1, &mergeSort);
            heap += timeit(original, arr, len, &heapSort);
            hybrid += timeit2(original, arr, 0, len - 1, &hybridSort);
        }

        std::cout
            << len << " "
            << ins.count() / times << " "
            << quick.count() / times << " "
            << heap.count() / times << " "
            << merge.count() / times << " "
            << hybrid.count() / times << " "
            << std::endl;

        delete[] arr;
        delete[] sorted;
        delete[] original;
    }


    return 0;
}


