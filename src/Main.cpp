#include "cafe_app.hpp"

// Главное меню приложения
int main() {
    //setlocale(LC_ALL, "Russian");
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    CafeApp app("cafe.db"); 

    while (true) {
        if (!app.isLoggedIn()) {
            std::cout << "\n=== Добро пожаловать в hotcoffe-now ===\n";
            std::cout << "1. Войти в систему\n";
            std::cout << "2. Зарегистрироваться\n";
            std::cout << "3. Выйти из программы\n";
            std::cout << "Выберите действие: ";
            int action; std::cin >> action;

            if (action == 1) {
                std::string user, pass;
                std::cout << "Имя пользователя: "; std::cin >> user;
                std::cout << "Пароль: "; std::cin >> pass;
                if (app.loginUser(user, pass)) {
                    std::cout << "Успешный вход!\n";
                } else {
                    std::cout << "Неверное имя пользователя или пароль.\n";
                }
            } else if (action == 2) {
                app.registerUser();
            } else if (action == 3) {
                break;
            }
        } else {
            std::cout << "\n=== Главное Меню Системы ===\n";
            int role = app.getCurrentRole();

            if (role == 1) { // ОФИЦИАНТ
                std::cout << "1. Разместить заказ (выбрать из меню)\n";
                std::cout << "2. Проверить готовые заказы\n";
                std::cout << "3. Выйти из аккаунта\n";
                std::cout << "Действие: ";
                int act; std::cin >> act;
                if (act == 1) app.placeOrder();
                    else if (act == 2) app.checkReadyOrders();
                    else if (act == 3) app.logout();
                }
                else if (role == 2) { // ПОВАР
                    std::cout << "1. Поиск новых заказов / Просмотр кухни\n";
                    std::cout << "2. Изменить статус заказа\n";
                    std::cout << "3. Выйти из аккаунта\n";
                    std::cout << "Действие: ";
                    int act; std::cin >> act;
                if (act == 1) app.checkNewOrders();
                else if (act == 2) app.changeOrderStatus();
                else if (act == 3) app.logout();
                }
                else if (role == 3) { // УПРАВЛЯЮЩИЙ
                    std::cout << "1. Управление персоналом (Добавить/Убрать)\n";
                    std::cout << "2. Зарегистрировать нового сотрудника\n";
                    std::cout << "3. Выйти из аккаунта\n";
                    std::cout << "Действие: ";
                    int act; std::cin >> act;
                    if (act == 1) app.managePersonnel();
                    else if (act == 2) app.registerUser();
                    else if (act == 3) app.logout();
                }
            }
        }
return 0;
}