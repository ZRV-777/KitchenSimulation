#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <ctime>
#include <chrono>
#include <random>

using namespace std;

// Мьютекс для синхронизации доступа к кухне
mutex kitchenMutex;

// Очереди для заказов
queue<string> incomingOrders;  // Заказы от клиентов
queue<string> readyOrders;     // Готовые блюда

// Счетчик успешных доставок
int deliveryCount = 0;

// Массив возможных блюд
const string dishes[] = { "Pizza", "Soup", "Steak", "Salad", "Sushi" };
const int NUM_DISHES = 5;

// Функция для симуляции работы кухни
void cookOrder(const string& order) {
    unique_lock<mutex> lock(kitchenMutex);

    // Симулируем время приготовления
    int cookTime = 5 + rand() % 11;  // 5-15 секунд
    this_thread::sleep_for(chrono::seconds(cookTime));

    // Добавляем готовое блюдо в очередь
    readyOrders.push(order);
    cout << "The finished dish: " << order << " finish for " << cookTime << " seconds" << endl;
}

// Функция для обработки заказов
void handleOrders() {
    while (deliveryCount < 10) {
        // Ждем поступления нового заказа
        while (incomingOrders.empty()) {
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        string order = incomingOrders.front();
        incomingOrders.pop();

        cout << "Get order: " << order << endl;

        // Запускаем нить для приготовления
        thread cookingThread(cookOrder, order);
        cookingThread.detach();
    }
}

// Функция для курьера
void deliveryService() {
    while (deliveryCount < 10) {
        this_thread::sleep_for(chrono::seconds(30));  // Интервал между доставками

        // Проверяем наличие готовых блюд
        while (!readyOrders.empty()) {
            string order = readyOrders.front();
            readyOrders.pop();

            deliveryCount++;
            cout << "The delivery: " << order << " successfully delivered (" << deliveryCount << "/10)" << endl;

            if (deliveryCount >= 10) {
                break;
            }
        }
    }
}

int main() {
    // Инициализация генератора случайных чисел
    srand(time(0));

    // Запускаем поток для обработки заказов
    thread orderHandler(handleOrders);
    orderHandler.detach();

    // Запускаем поток для курьера
    thread deliveryThread(deliveryService);
    deliveryThread.detach();

    // Симулируем поступление заказов
    while (deliveryCount < 10) {
        // Случайный интервал между заказами (5-10 секунд)
        int orderInterval = 5 + rand() % 6;
        this_thread::sleep_for(chrono::seconds(orderInterval));

        // Случайное блюдо
        int dishIndex = rand() % NUM_DISHES;
        string order = dishes[dishIndex];

        // Добавляем заказ в очередь
        incomingOrders.push(order);
    }

    cout << "\nAll 10 deliveries have been completed successfully!" << endl;

    return 0;
}