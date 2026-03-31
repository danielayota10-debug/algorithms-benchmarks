#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//  ANSI colors

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define WHITE   "\033[97m"

//  Helpers

void swap(int *a, int *b) { int t = *a; *a = *b; *b = t; }

int *copy_array(const int *src, int n) {
    int *dst = malloc(n * sizeof(int));
    memcpy(dst, src, n * sizeof(int));
    return dst;
}

double elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0
         + (end.tv_nsec - start.tv_nsec) / 1e6;
}

int compare_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

//---------------------------------------
//  Sorting algorithms
//---------------------------------------

// Insertion Sort
void insertion_sort(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i], j = i - 1;
        while (j >= 0 && arr[j] > key) { arr[j+1] = arr[j]; j--; }
        arr[j+1] = key;
    }
}

// Bubble Sort
void bubble_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j+1]) swap(&arr[j], &arr[j+1]);
}

// Selection Sort
void selection_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int mn = i;
        for (int j = i+1; j < n; j++) if (arr[j] < arr[mn]) mn = j;
        swap(&arr[i], &arr[mn]);
    }
}

// Merge Sort
void merge(int *arr, int l, int m, int r) {
    int n1 = m-l+1, n2 = r-m;
    int *L = malloc(n1*sizeof(int)), *R = malloc(n2*sizeof(int));
    for (int i=0;i<n1;i++) L[i]=arr[l+i];
    for (int j=0;j<n2;j++) R[j]=arr[m+1+j];
    int i=0,j=0,k=l;
    while(i<n1&&j<n2) arr[k++]=(L[i]<=R[j])?L[i++]:R[j++];
    while(i<n1) arr[k++]=L[i++];
    while(j<n2) arr[k++]=R[j++];
    free(L); free(R);
}
void merge_sort_rec(int *arr, int l, int r) {
    if (l<r) { int m=(l+r)/2; merge_sort_rec(arr,l,m); merge_sort_rec(arr,m+1,r); merge(arr,l,m,r); }
}
void merge_sort(int *arr, int n) { if (n>1) merge_sort_rec(arr,0,n-1); }

// Quick Sort
int partition(int *arr, int low, int high) {
    int pivot=arr[high], i=low-1;
    for(int j=low;j<high;j++) if(arr[j]<=pivot) swap(&arr[++i],&arr[j]);
    swap(&arr[i+1],&arr[high]); return i+1;
}
void quick_sort_rec(int *arr, int low, int high) {
    if(low<high){ int pi=partition(arr,low,high); quick_sort_rec(arr,low,pi-1); quick_sort_rec(arr,pi+1,high); }
}
void quick_sort(int *arr, int n) { if(n>1) quick_sort_rec(arr,0,n-1); }

// Heap Sort
void heapify(int *arr, int n, int i) {
    int largest=i, l=2*i+1, r=2*i+2;
    if(l<n&&arr[l]>arr[largest]) largest=l;
    if(r<n&&arr[r]>arr[largest]) largest=r;
    if(largest!=i){ swap(&arr[i],&arr[largest]); heapify(arr,n,largest); }
}
void heap_sort(int *arr, int n) {
    for(int i=n/2-1;i>=0;i--) heapify(arr,n,i);
    for(int i=n-1;i>0;i--){ swap(&arr[0],&arr[i]); heapify(arr,i,0); }
}

//  UI helpers

void print_banner() {
    printf("\n");
    printf(CYAN BOLD "  ╔══════════════════════════════════════════════╗\n" RESET);
    printf(CYAN BOLD "  ║   " WHITE "SORTING ALGORITHM BENCHMARK" CYAN "  ·  " YELLOW "C" CYAN "          ║\n" RESET);
    printf(CYAN BOLD "  ╚══════════════════════════════════════════════╝\n" RESET);
    printf("\n");
}

void print_bar(double ms, double max_ms, int bar_width) {
    int filled = (max_ms > 0) ? (int)(ms / max_ms * bar_width) : 0;
    if (filled > bar_width) filled = bar_width;
    printf(GREEN "  [");
    for (int i = 0; i < bar_width; i++)
        printf(i < filled ? "█" : DIM "░" RESET GREEN);
    printf("]" RESET);
}

void print_result(const char *name, double ms, double max_ms, int is_skipped) {
    if (is_skipped) {
        printf(YELLOW "  %-18s " DIM "skipped (too slow for large N)\n" RESET, name);
        return;
    }
    printf(WHITE BOLD "  %-18s " RESET, name);
    if (ms < 1.0)       printf(GREEN  "  %8.3f ms  " RESET, ms);
    else if (ms < 100)  printf(YELLOW "  %8.3f ms  " RESET, ms);
    else                printf(RED    "  %8.3f ms  " RESET, ms);
    print_bar(ms, max_ms, 30);
    printf("\n");
}

void separator() { printf(DIM "  ──────────────────────────────────────────────\n" RESET); }

//  File loader

int *load_from_file(const char *path, int *out_n) {
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, RED "  Error: cannot open '%s'\n" RESET, path); return NULL; }

    int cap = 1024, n = 0;
    int *arr = malloc(cap * sizeof(int));
    int v;
    while (fscanf(f, "%d", &v) == 1) {
        if (n == cap) { cap *= 2; arr = realloc(arr, cap * sizeof(int)); }
        arr[n++] = v;
    }
    fclose(f);
    *out_n = n;
    return arr;
}

int *generate_random(int n) {
    int *arr = malloc(n * sizeof(int));
    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++) arr[i] = rand() % (n * 10);
    return arr;
}


//  Benchmark runner

typedef void (*sort_fn)(int*, int);

typedef struct { const char *name; sort_fn fn; int skip_large; } Algorithm;

#define LARGE_THRESHOLD 100000

void run_benchmarks(int *original, int n) {
    Algorithm algos[] = {
        {"Insertion Sort", insertion_sort,  1},
        {"Bubble Sort",    bubble_sort,     1},
        {"Selection Sort", selection_sort,  1},
        {"Merge Sort",     merge_sort,      0},
        {"Quick Sort",     quick_sort,      0},
        {"Heap Sort",      heap_sort,       0},
    };
    int count = sizeof(algos) / sizeof(algos[0]);

    printf(DIM "  Array size: " RESET WHITE BOLD "%d elements\n\n" RESET, n);

    double times[6];
    double max_ms = 0;

    for (int i = 0; i < count; i++) {
        int skip = algos[i].skip_large && n > LARGE_THRESHOLD;
        if (skip) { times[i] = -1; continue; }

        int *arr = copy_array(original, n);
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        algos[i].fn(arr, n);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        times[i] = elapsed_ms(t0, t1);
        if (times[i] > max_ms) max_ms = times[i];
        free(arr);
    }

    separator();
    printf(DIM "  %-18s   %10s   %s\n" RESET, "Algorithm", "Time", "Relative");
    separator();

    for (int i = 0; i < count; i++)
        print_result(algos[i].name, times[i], max_ms, times[i] < 0);

    separator();

    // Find winner
    double best = 1e18; int best_i = -1;
    for (int i = 0; i < count; i++)
        if (times[i] >= 0 && times[i] < best) { best = times[i]; best_i = i; }

    if (best_i >= 0)
        printf(GREEN BOLD "\n  🏆 Fastest: %s (%.3f ms)\n" RESET, algos[best_i].name, best);
    printf("\n");
}

//  Main
// ─────────────────────────────────────────────

int main(int argc, char *argv[]) {
    print_banner();

    int n = 0;
    int *data = NULL;
    char choice[8];

    if (argc >= 2) {
        // File path passed as argument
        data = load_from_file(argv[1], &n);
        if (!data) return 1;
        printf(GREEN "  ✔ Loaded %d integers from '%s'\n\n" RESET, n, argv[1]);
    } else {
        printf(BOLD "  Input source:\n" RESET);
        printf("    " CYAN "[1]" RESET " Generate random array\n");
        printf("    " CYAN "[2]" RESET " Load from file\n");
        printf("\n  Choice: ");
        fgets(choice, sizeof(choice), stdin);

        if (choice[0] == '2') {
            char path[512];
            printf("  File path: ");
            fgets(path, sizeof(path), stdin);
            path[strcspn(path, "\n")] = 0;
            data = load_from_file(path, &n);
            if (!data) return 1;
            printf(GREEN "  ✔ Loaded %d integers\n\n" RESET, n);
        } else {
            printf("  Array size (e.g. 100000): ");
            scanf("%d", &n);
            if (n <= 0) { fprintf(stderr, RED "  Invalid size.\n" RESET); return 1; }
            data = generate_random(n);
            printf(GREEN "  ✔ Generated %d random integers\n\n" RESET, n);
        }
    }

    if (n > LARGE_THRESHOLD)
        printf(YELLOW "  ⚠  N > %d → O(n²) algorithms will be skipped.\n\n" RESET, LARGE_THRESHOLD);

    printf(CYAN BOLD "  Running benchmarks...\n\n" RESET);
    run_benchmarks(data, n);

    free(data);
    return 0;
}
