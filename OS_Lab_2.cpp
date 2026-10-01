#include <iostream>
#include <vector>
#include <windows.h>

typedef void (*ReplaceMinMaxFunc)(int*, int, int, int, int);

struct ThreadData {
    std::vector<int>* arr;
    int min_val;
    int max_val;
    int average_val;
};

DWORD WINAPI MinMaxThread(LPVOID lpParam) {
    ULONGLONG start_time = GetTickCount64();

    ThreadData* data = static_cast<ThreadData*>(lpParam);
    const std::vector<int>& arr = *(data->arr);

    if (arr.empty()) return 0;

    int min_v = arr[0];
    int max_v = arr[0];

    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < min_v) min_v = arr[i];
        Sleep(7);

        if (arr[i] > max_v) max_v = arr[i];
        Sleep(7);
    }

    data->min_val = min_v;
    data->max_val = max_v;

    ULONGLONG end_time = GetTickCount64();

    std::cout << "[min_max] Минимальный: " << min_v << ", Максимальный: " << max_v
        << " | Время: " << (end_time - start_time) << " мс" << std::endl;

    return 0;
}

DWORD WINAPI AverageThread(LPVOID lpParam) {
    ULONGLONG start_time = GetTickCount64();

    ThreadData* data = static_cast<ThreadData*>(lpParam);
    const std::vector<int>& arr = *(data->arr);

    if (arr.empty()) return 0;

    long long sum = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        sum += arr[i];
        Sleep(12);
    }

    data->average_val = static_cast<int>(sum / arr.size());
    double exact_avg = static_cast<double>(sum) / arr.size();

    ULONGLONG end_time = GetTickCount64();

    std::cout << "[average] Среднее: " << exact_avg
        << " | Время: " << (end_time - start_time) << " мс" << std::endl;

    return 0;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int n;
    std::cout << "Введите размерность массива: ";
    std::cin >> n;

    if (n <= 0) {
        std::cout << "Размерность массива должна быть больше 0." << std::endl;
        return 1;
    }

    std::vector<int> arr(n);
    std::cout << "Введите " << n << " целых чисел: ";
    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }

    ThreadData data;
    data.arr = &arr;

    HANDLE hMinMax = CreateThread(NULL, 0, MinMaxThread, &data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, AverageThread, &data, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        std::cerr << "Ошибка при создании потоков!" << std::endl;
        return 1;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    HMODULE hLib = LoadLibraryA("test.dll");
    if (hLib == NULL) {
        std::cerr << "Ошибка: не удалось загрузить test.dll!" << std::endl;
        return 1;
    }

    ReplaceMinMaxFunc replaceFunc = (ReplaceMinMaxFunc)GetProcAddress(hLib, "ReplaceMinMax");
    if (replaceFunc == NULL) {
        std::cerr << "Ошибка: функция ReplaceMinMax не найдена!" << std::endl;
        FreeLibrary(hLib);
        return 1;
    }

    replaceFunc(arr.data(), n, data.min_val, data.max_val, data.average_val);
    FreeLibrary(hLib);

    std::cout << "\n[main] Итоговый массив: ";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
