#define _CRT_SECURE_NO_WARNINGS
#include <iostream>  
#include <string>  
#include <mutex>  
#include <fstream>  
#include <vector>  
#include <ctime>  
#include <algorithm> 
using namespace std;

// Структура для хранения данных клиента  
struct Client {
    int id;
    string name;
    string phone;
    string email;
};

class DatabaseClient {
private:
    // Мутекс для потокобезопасности  
    mutable mutex mtx;
    // Файловый поток для логирования  
    ofstream logFile;
    // Вектор для хранения данных (имитация БД)  
    vector<Client> database;
    // Флаг подключения  
    bool connected;
    // Функция логирования  
    void logUnsafe(const string& message) {
        time_t now = time(0);
        logFile << ctime(&now) << " " << message << endl;
    }

public:
    // Конструктор - устанавливаем соединение  
    DatabaseClient() : connected(false) {
        try {
            // Имитация подключения к БД  
            connected = true;
            logFile.open("database.log", ios::app);
            logUnsafe("Подключение к базе данных установлено");
        }
        catch (...) {
            connected = false;
            logUnsafe("Ошибка при подключении к базе данных");
            throw;
        }
    }
    // Деструктор - освобождаем ресурсы  
    ~DatabaseClient() {
        if (connected) {
            logUnsafe("Соединение с базой данных закрыто");
            logFile.close();
        }
    }
    // Добавление клиента  
    void addClient(const Client& client) {
        lock_guard<mutex> lock(mtx);
        if (connected) {
            database.push_back(client);
            logUnsafe("Добавлен клиент: " + client.name);
        }
        else {
            logUnsafe("Ошибка добавления клиента - нет подключения");
        }
    }
    // Поиск клиента по ID  
    Client findClientById(int id) {
        lock_guard<mutex> lock(mtx);
        if (connected) {
            for (const auto& client : database) {
                if (client.id == id) {
                    logUnsafe("Найден клиент с ID: " + to_string(id));
                    return client;
                }
            }
        }
        logUnsafe("Клиент с ID " + to_string(id) + " не найден");
        return Client{ -1, "", "", "" };
    }

    // Удаление клиента по ID  
    void removeClientById(int id) {
        lock_guard<mutex> lock(mtx);
        if (connected) {
            auto it = remove_if(database.begin(), database.end(),  [id](const Client& c) { return c.id == id; });
            if (it != database.end()) {
                database.erase(it, database.end());
                logUnsafe("Удален клиент с ID: " + to_string(id));
            }
            else {
                logUnsafe("Ошибка удаления клиента с ID: " + to_string(id));
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "");
    try {
        DatabaseClient db;

        // Пример использования  
        Client client1 = { 1, "Иван Иванов", "123-45-67", "ivanov@mail.ru" };
        db.addClient(client1);

        Client found = db.findClientById(1);
        if (found.id != -1) {
            cout << "Найден клиент: " << found.name << endl;
        }

        db.removeClientById(1);
    }
    catch (const exception& e) {
        cerr << "Произошла ошибка: " << e.what() << endl;
    }

    return 0;
}