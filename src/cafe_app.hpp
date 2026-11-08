#pragma once

#include "init_db.hpp"

#include <iostream>
#include <fstream>
//#include <string>
//#include <vector>
#include <functional>
//#include <clocale>

extern "C" {
    #include <sqlite3.h>
}

// Структуры для удобства работы с данными
struct MenuItem {
    int id;
    std::string name;
    double price;
};

struct User {
    int id;
    std::string username;
    int role;
};

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.is_open();
}

// Класс для работы с БД Кафе
class CafeApp {
private:
    sqlite3* db;
    User currentUser{0, "", 0}; // Авторизованный пользователь

    // Хэширование пароля (для демонстрации используем стандартный std::hash)
    int hashPassword(const std::string& password) {
        std::hash<std::string> hasher;
        return static_cast<int>(hasher(password) % 10000); // Ограничим длину для простоты
    }

    // Вспомогательный метод для выполнения SQL без возврата строк
    bool executeSQL(const std::string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Ошибка SQL: " << errMsg << "\n";
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

public:
    CafeApp(const std::string& db_path) {
        bool init = fileExists(db_path);
        if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Не удалось открыть базу данных: " << sqlite3_errmsg(db) << "\n";
            exit(1);
        }
        if (!init) {
            char* errorMessage = nullptr;
            int rc = sqlite3_exec(db, initDb.c_str(), nullptr, nullptr, &errorMessage);
            
            if (rc != SQLITE_OK) {
                std::cerr << "Ошибка SQL: " << errorMessage << std::endl;
                sqlite3_free(errorMessage); // Обязательно освобождаем память ошибки
                exit(1);
            } 
            std::cout << "Импорт файла успешно завершен!" << std::endl;   
        }
    }

    ~CafeApp() {
        sqlite3_close(db);
    }

    bool isLoggedIn() { return currentUser.id != 0; }
    int getCurrentRole() { return currentUser.role; }
    void logout() { currentUser = {0, "", 0}; std::cout << "Вы вышли из системы.\n"; }

    // --- ВХОД И РЕГИСТРАЦИЯ ---
    bool loginUser(const std::string& username, const std::string& password) {
        // ОБНОВЛЕНО: Используется таблица USER_data
        std::string sql = "SELECT user_id, user_role FROM USER_data WHERE username = '" + username + 
                          "' AND password_hash = " + std::to_string(hashPassword(password)) + ";";
        
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                currentUser.id = sqlite3_column_int(stmt, 0);
                currentUser.username = username;
                currentUser.role = sqlite3_column_int(stmt, 1);
                sqlite3_finalize(stmt);
                return true;
            }
        }
        sqlite3_finalize(stmt);
        return false;
    }

    void registerUser() {
        std::string name, pass;
        int role;
        std::cout << "Введите имя пользователя: "; std::cin >> name;
        std::cout << "Введите пароль: "; std::cin >> pass;
        std::cout << "Выберите роль (1 - Официант, 2 - Повар, 3 - Управляющий): "; std::cin >> role;

        // Поиск свободного ID
        int next_id = 1;
        sqlite3_stmt* stmt;
        // ОБНОВЛЕНО: Используется таблица USER_data
        if (sqlite3_prepare_v2(db, "SELECT MAX(user_id) FROM USER_data;", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) next_id = sqlite3_column_int(stmt, 0) + 1;
        }
        sqlite3_finalize(stmt);

        // ОБНОВЛЕНО: Используется таблица USER_data
        std::string sql = "INSERT INTO USER_data (user_id, username, password_hash, user_role) VALUES (" +
                          std::to_string(next_id) + ", '" + name + "', " + std::to_string(hashPassword(pass)) + ", " + std::to_string(role) + ");";
        
        if (executeSQL(sql)) std::cout << "Пользователь успешно зарегистрирован!\n";
    }

    // --- ФУНКЦИИ ОФИЦИАНТА ---
    void showMenuPositions() {
        std::cout << "\n--- Меню Доступных Позиций ---\n";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, "SELECT order_id, name, price FROM \"order\";", -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::cout << "[" << sqlite3_column_int(stmt, 0) << "] " 
                          << sqlite3_column_text(stmt, 1) << " | Цена: " 
                          << sqlite3_column_double(stmt, 2) << " руб.\n";
            }
        }
        sqlite3_finalize(stmt);
    }

    void placeOrder() {
        showMenuPositions();
        int order_id;
        std::cout << "Выберите ID позиции для заказа: "; std::cin >> order_id;

        int next_menu_id = 1;
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, "SELECT MAX(menu_id) FROM menu;", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) next_menu_id = sqlite3_column_int(stmt, 0) + 1;
        }
        sqlite3_finalize(stmt);

        std::string sql = "INSERT INTO menu (menu_id, user_id, order_id, order_status) VALUES (" +
                          std::to_string(next_menu_id) + ", " + std::to_string(currentUser.id) + ", " + 
                          std::to_string(order_id) + ", 1);";
        
        if (executeSQL(sql)) std::cout << "Заказ успешно размещен (Статус: Новый)!\n";
    }

    void checkReadyOrders() {
        std::cout << "\n--- Готовые заказы (Ждут выдачи) ---\n";
        std::string sql = "SELECT m.menu_id, o.name FROM menu m "
                          "JOIN \"order\" o ON m.order_id = o.order_id "
                          "WHERE m.order_status = 3 AND m.user_id = " + std::to_string(currentUser.id) + ";";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::cout << "Заказ ID: " << sqlite3_column_int(stmt, 0) << " | Блюдо: " << sqlite3_column_text(stmt, 1) << "\n";
            }
        }
        sqlite3_finalize(stmt);
    }

    // --- ФУНКЦИИ ПОВАРА ---
    void checkNewOrders() {
        std::cout << "\n--- Новые и активные заказы на кухне ---\n";
        std::string sql = "SELECT m.menu_id, o.name, s.name FROM menu m "
                          "JOIN \"order\" o ON m.order_id = o.order_id "
                          "JOIN order_status s ON m.order_status = s.status_id "
                          "WHERE m.order_status IN (1, 2);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::cout << "Заказ ID: " << sqlite3_column_int(stmt, 0) 
                          << " | Блюдо (Загружено из меню): " << sqlite3_column_text(stmt, 1) 
                          << " | Статус: " << sqlite3_column_text(stmt, 2) << "\n";
            }
        }
        sqlite3_finalize(stmt);
    }

    void changeOrderStatus() {
        checkNewOrders();
        int menu_id, new_status;
        std::cout << "Введите ID заказа для изменения: "; std::cin >> menu_id;
        std::cout << "Выберите новый статус (2 - Готовится, 3 - Готов): "; std::cin >> new_status;

        std::string sql = "UPDATE menu SET order_status = " + std::to_string(new_status) + 
                          " WHERE menu_id = " + std::to_string(menu_id) + ";";
        if (executeSQL(sql)) std::cout << "Статус заказа успешно изменен!\n";
    }

    // --- ФУНКЦИИ УПРАВЛЯЮЩЕГО (АДМИНА) ---
    void managePersonnel() {
        std::cout << "\n--- Управление персоналом ---\n";
        std::cout << "1. Посмотреть весь персонал\n";
        std::cout << "2. Удалить сотрудника\n";
        int choice; std::cin >> choice;

        if (choice == 1) {
            sqlite3_stmt* stmt;
            // ОБНОВЛЕНО: Используется таблица USER_data
            std::string sql = "SELECT u.user_id, u.username, r.name FROM USER_data u JOIN USER_ROLE r ON u.user_role = r.user_role;";
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    std::cout << "ID: " << sqlite3_column_int(stmt, 0) << " | Логин: " 
                              << sqlite3_column_text(stmt, 1) << " | Роль: " << sqlite3_column_text(stmt, 2) << "\n";
                }
            }
            sqlite3_finalize(stmt);
        } else if (choice == 2) {
            int del_id;
            std::cout << "Введите ID сотрудника для удаления: "; std::cin >> del_id;
            // ОБНОВЛЕНО: Используется таблица USER_data
            std::string sql = "DELETE FROM USER_data WHERE user_id = " + std::to_string(del_id) + ";";
            if (executeSQL(sql)) std::cout << "Сотрудник удален из системы.\n";
        }
    }
};