#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <boost/thread.hpp>
using namespace std;

boost::mutex mtx;
long long counter1, counter3 = 0;
std::atomic<long long> counter2(0);

void sumFinderMutex(vector<int>& numbers, int start, int end) {
	for (int i = start; i < end; ++i) {
		std::lock_guard<boost::mutex> lock(mtx);
		counter1 += numbers[i];
	}
}

void sumFinderAtomic(vector<int>& numbers, int start, int end) {
	for (int i = start; i < end; ++i) {
			counter2 += numbers[i];
	}
}

void sumFinderNone(vector<int>& numbers, int start, int end) {
	for (int i = start; i < end; ++i) {
			counter3 += numbers[i];
	}
}

double createThreads(void (*functionName)(vector<int>&, int, int), int N, int threadsNumber, vector<int>& numbers) {

	vector<boost::thread> threads;
	int blockSize  = N / threadsNumber;
	int remainder  = N  % threadsNumber;
	int start = 0;

	auto startTime = chrono::high_resolution_clock::now();
	for (int i = 0; i < threadsNumber; i++) {
		int end = start + blockSize;

		if (i < remainder) ++end;
		if (end > N) end = N;
		if (start >= end) break;

		threads.push_back(boost::thread(functionName, std::ref(numbers), start, end));
		start = end;
	}

	for (auto& t : threads) t.join();
	auto endTime = chrono::high_resolution_clock::now();

	return chrono::duration<double>(endTime - startTime).count();

}

int main() {
	int N, threadsNumber;
	vector<int> numbers;

	cout << "Количество чисел в массиве: ";
	cin >> N;
	cout << "Количество потоков: ";
	cin >> threadsNumber;

	for (int i = 1; i <= N; i++) {
		numbers.push_back(i);
	}

	double timeMutex = createThreads(sumFinderMutex, N, threadsNumber, std::ref(numbers));
	double timeAtomic = createThreads(sumFinderAtomic, N, threadsNumber, std::ref(numbers));
	double timeNone = createThreads(sumFinderNone, N, threadsNumber, std::ref(numbers));


    cout << "\n";
	cout << "Мьютекс \n";
	cout << "Сумма: " << counter1 << "\n";
	cout << "Время в секундах: " << timeMutex << "\n";
	cout << "\n";

	cout << "Атомарная переменая \n";
	cout << "Сумма: " << counter2 << "\n";
	cout << "Время в секундах: " << timeAtomic << "\n";
	cout << "\n";

	cout << "Ничего \n";
	cout << "Сумма: " << counter3 << "\n";
	cout << "Время в секундах: " << timeNone << "\n";

	return 0;
}
