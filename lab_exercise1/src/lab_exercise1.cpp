#include <iostream>
#include <algorithm>
#include <cmath>
#include <boost/thread.hpp>
#include <vector>
#include <chrono>

boost::mutex mtx;
using namespace std;

void simpleNumberFinder(vector<int>& numbers, int start, int end) {
    for (int i = start; i < end && i < (int)numbers.size(); ++i) {
        if (numbers[i] == -1) continue;

        int p = numbers[i];

        int sq = p * p;
        if (sq > numbers.size()) continue;

        int squareIdx = sq - 1;

        for (int j = squareIdx; j < numbers.size(); j += p) {
        	std::lock_guard<boost::mutex> lock(mtx);
            numbers[j] = -1;
        }
    }
}

int main() {
    int N, threadsNumber;
    vector<int> answer;
    vector<boost::thread> threads;

    cout << "Найти все простые числа от 1 до: ";
    cin >> N;
    cout << "Количество потоков: ";
    cin >> threadsNumber;

    int sqrtN = (int)sqrt(N);

    vector<int> numbers(N);
    for (int i = 0; i < N; ++i) numbers[i] = i + 1;
    numbers[0] = -1;

    simpleNumberFinder(numbers, 0, sqrtN + 1);

    auto startTime = chrono::high_resolution_clock::now();

    int blockSize  = (N - sqrtN) / threadsNumber;
    int remainder  = (N - sqrtN) % threadsNumber;

    int start = sqrtN;
    for (int i = 0; i < threadsNumber; ++i) {
        int end = start + blockSize;
        if (i < remainder) ++end;
        if (end > N) end = N;

        if (start >= end) break;

        threads.push_back(boost::thread(simpleNumberFinder, ref(numbers), start, end));

        start = end;
    }

    for (auto& t : threads) t.join();

    auto endTime = chrono::high_resolution_clock::now();

    for (int i = 1; i < N; ++i) {
        if (numbers[i] != -1) {
            answer.push_back(numbers[i]);
        }
    }

    cout << "Все простые числа: ";
    for (size_t i = 0; i < answer.size(); ++i) {
        cout << answer[i];
        if (i + 1 < answer.size()) cout << " ";
    }
    cout << "\n";

    cout << "Время: " << chrono::duration<double>(endTime - startTime).count() << " секунд\n";

    return 0;
}
