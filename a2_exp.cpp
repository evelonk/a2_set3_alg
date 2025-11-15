#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <string>

class ArrayGenerator {
 private:
  std::vector<int> randomData;
  std::vector<int> reversedData;
  std::vector<int> nearlySortedData;

 public:
  ArrayGenerator() {
    randomData.resize(100000);
    reversedData.resize(100000);
    nearlySortedData.resize(100000);

    std::mt19937 rng(1234567);
    std::uniform_int_distribution<int> dist(0, 10000);

    for (int i = 0; i < 100000; i++) {
      randomData[i] = dist(rng);
    }
    for (int i = 0; i < 100000; i++) {
      reversedData[i] = 100000 - i;
    }
    for (int i = 0; i < 100000; i++) {
      nearlySortedData[i] = i;
    }
    for (int k = 0; k < 200; k++) {
      int i = dist(rng) % 100000;
      int j = dist(rng) % 100000;
      std::swap(nearlySortedData[i], nearlySortedData[j]);
    }
  }

  std::vector<int> getRandom(int n) {
    return std::vector<int>(randomData.begin(), randomData.begin() + n);
  }

  std::vector<int> getReversed(int n) {
    return std::vector<int>(reversedData.begin(), reversedData.begin() + n);
  }

  std::vector<int> getNearlySorted(int n) {
    return std::vector<int>(nearlySortedData.begin(), nearlySortedData.begin() + n);
  }
};

void insertionSort(std::vector<int>& a, int l, int r) {
  for (int i = l + 1; i <= r; ++i) {
    int cur = a[i];
    int j = i - 1;
    while (j >= l && a[j] > cur) {
      a[j + 1] = a[j];
      j--;
    }
    a[j + 1] = cur;
  }
}

void mergeSort(std::vector<int>& a, std::vector<int>& t, int l, int r) {
  if (l >= r) {
    return;
  }
  int mid = (l + r) / 2;
  mergeSort(a, t, l, mid);
  mergeSort(a, t, mid + 1, r);
  int i = l;
  int j = mid + 1;
  int k = l;
  while (i <= mid && j <= r) {
    if (a[i] <= a[j]) {
      t[k++] = a[i++];
    } else {
      t[k++] = a[j++];
    }
  }
  while (i <= mid) {
    t[k++] = a[i++];
  }
  while (j <= r) {
    t[k++] = a[j++];
  }
  for (int p = l; p <= r; p++) {
    a[p] = t[p];
  }
}

void hybridMergeSort(std::vector<int>& a, std::vector<int>& t, int l, int r, int lim) {
  if (r - l + 1 <= lim) {
    insertionSort(a, l, r);
    return;
  }
  int mid = (l + r) / 2;
  hybridMergeSort(a, t, l, mid, lim);
  hybridMergeSort(a, t, mid + 1, r, lim);
  int i = l, j = mid + 1, k = l;
  while (i <= mid && j <= r) {
    if (a[i] <= a[j])
      t[k++] = a[i++];
    else
      t[k++] = a[j++];
  }
  while (i <= mid) {
    t[k++] = a[i++];
  }
  while (j <= r) {
    t[k++] = a[j++];
  }
  for (int p = l; p <= r; p++) {
    a[p] = t[p];
  }
}

class SortTester {
 private:
  int runs;

 public:
  SortTester(int runs = 5) : runs(runs) {
  }

  int testMergeSort(std::vector<int>& base) {
    int total = 0;
    for (int r = 0; r < runs; r++) {
      std::vector<int> a = base;
      std::vector<int> temp(a.size());
      auto start = std::chrono::high_resolution_clock::now();
      mergeSort(a, temp, 0, a.size() - 1);
      auto end = std::chrono::high_resolution_clock::now();
      total += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
    return total / runs;
  }

  int testHybrid(std::vector<int>& base, int lim) {
    int total = 0;
    for (int r = 0; r < runs; r++) {
      std::vector<int> a = base;
      std::vector<int> t(a.size());
      auto start = std::chrono::high_resolution_clock::now();
      hybridMergeSort(a, t, 0, a.size() - 1, lim);
      auto end = std::chrono::high_resolution_clock::now();
      total += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
    return total / runs;
  }
};

void runAndSave(const std::string& filename, const std::vector<int>& base, SortTester& tester) {
  std::ofstream out(filename);
  out << "n,merge,hybrid5,hybrid10,hybrid20,hybrid30,hybrid50\n";
  for (int n = 500; n <= 100000; n += 100) {
    std::vector<int> data(base.begin(), base.begin() + n);
    int t_merge = tester.testMergeSort(data);
    int t_h5 = tester.testHybrid(data, 5);
    int t_h10 = tester.testHybrid(data, 10);
    int t_h20 = tester.testHybrid(data, 20);
    int t_h30 = tester.testHybrid(data, 30);
    int t_h50 = tester.testHybrid(data, 50);
    out << n << "," << t_merge << "," << t_h5 << "," << t_h10 << "," << t_h20 << "," << t_h30 << "," << t_h50 << "\n";
  }
}

int main() {
  ArrayGenerator gen;
  SortTester tester(5);
  runAndSave("random.csv", gen.getRandom(100000), tester);
  runAndSave("reversed.csv", gen.getReversed(100000), tester);
  runAndSave("nearly.csv", gen.getNearlySorted(100000), tester);
  return 0;
}
