#include <stdio.h>
#include <string.h>

#define MAX 40000
#define K_MAX 1000

typedef struct {
    char id[30];
    char name[100];
    char gender[10];
    int korean, english, math, total;
} Student;

Student original[MAX], sorted[MAX];
long long quickCount = 0, seqCount = 0, binaryCount = 0;
long long seqResult[K_MAX + 1], binaryResult[K_MAX + 1];

/* 비교 함수 호출 한 번을 비교 1회로 계산 */
int compare(char a[], char b[], long long *count) {
    (*count)++;
    return strcmp(a, b);
}

void printStudent(Student s) {
    printf("ID: %s  Name: %s  Korean: %d  English: %d  Math: %d  Total: %d\n",
           s.id, s.name, s.korean, s.english, s.math, s.total);
}

/* 중간 이름을 피벗으로 선택하는 퀵 정렬 */
void quickSort(int left, int right) {
    int i = left, j = right;
    char pivot[100];
    Student temp;

    strcpy(pivot, sorted[(left + right) / 2].name);
    while (i <= j) {
        while (compare(sorted[i].name, pivot, &quickCount) < 0) i++;
        while (compare(sorted[j].name, pivot, &quickCount) > 0) j--;
        if (i <= j) {
            temp = sorted[i];
            sorted[i] = sorted[j];
            sorted[j] = temp;
            i++;
            j--;
        }
    }
    if (left < j) quickSort(left, j);
    if (i < right) quickSort(i, right);
}

void sequentialSearch(int n, char name[], int show) {
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (compare(original[i].name, name, &seqCount) == 0) {
            if (show) printStudent(original[i]);
            found++;
        }
    }
    if (show) printf("Matches: %d\n", found);
}

void binarySearch(int n, char name[], int show) {
    int left = 0, right = n - 1;
    int first = n, found = 0;

    /* 이름이 name 이상인 첫 위치를 찾는다. */
    while (left <= right) {
        int mid = (left + right) / 2;
        int result = compare(sorted[mid].name, name, &binaryCount);
        if (result < 0) {
            left = mid + 1;
        } else {
            first = mid;
            right = mid - 1;
        }
    }

    /* 첫 위치부터 같은 이름의 학생을 모두 출력한다. */
    for (int i = first; i < n; i++) {
        if (compare(sorted[i].name, name, &binaryCount) != 0) break;
        if (show) printStudent(sorted[i]);
        found++;
    }
    if (show) printf("Matches: %d\n", found);
}

int main(void) {
    FILE *file = fopen("students.csv", "r");
    char header[300];
    char searchName[100];
    int n = 0, firstWin = 0, fields;
    Student s;

    if (file == NULL) {
        printf("Cannot open students.csv\n");
        return 1;
    }
    if (fgets(header, sizeof(header), file) == NULL) {
        fclose(file);
        return 1;
    }

    /* 제공된 CSV: 학번,이름,성별,국어,영어,수학 */
    while ((fields = fscanf(file, " %29[^,],%99[^,],%9[^,],%d,%d,%d",
                            s.id, s.name, s.gender,
                            &s.korean, &s.english, &s.math)) == 6) {
        if (n == MAX) {
            printf("Too many students\n");
            fclose(file);
            return 1;
        }
        s.total = s.korean + s.english + s.math;
        original[n] = s;
        sorted[n] = s;
        n++;
    }
    fclose(file);
    if (fields != EOF || n == 0) {
        printf("CSV format error\n");
        return 1;
    }

    printf("Enter student name: ");
    fflush(stdout);
    if (fgets(searchName, sizeof(searchName), stdin) == NULL) return 1;
    searchName[strcspn(searchName, "\r\n")] = '\0';
    if (searchName[0] == '\0') {
        printf("Please enter a name.\n");
        return 1;
    }

    printf("\n[Sequential Search]\n");
    sequentialSearch(n, searchName, 1);

    quickSort(0, n - 1);
    printf("\n[Binary Search]\n");
    binarySearch(n, searchName, 1);

    /* 직접 검색과 K 실험의 비교 횟수를 분리한다. */
    seqCount = 0;
    binaryCount = 0;

    for (int k = 1; k <= K_MAX; k++) {
        char name[100];
        if (k % 5 == 0) strcpy(name, "__NO_SUCH_STUDENT__");
        else strcpy(name, original[(k - 1) % n].name);

        sequentialSearch(n, name, 0);
        binarySearch(n, name, 0);

        seqResult[k] = seqCount;
        binaryResult[k] = quickCount + binaryCount;
        if (firstWin == 0 && binaryResult[k] < seqResult[k]) firstWin = k;
    }

    printf("\nStudents: %d\n", n);
    printf("Quick Sort comparisons: %lld\n", quickCount);
    printf("\nK\tSequential\tQuick + Binary\n");
    for (int k = 1; k <= K_MAX; k++) {
        if (k <= 30 || k == 50 || k % 100 == 0)
            printf("%d\t%lld\t%lld\n", k, seqResult[k], binaryResult[k]);
    }
    if (firstWin != 0) printf("First winning K: %d\n", firstWin);
    else printf("No crossover up to K = %d\n", K_MAX);
    return 0;
}
