-- 1. Справочник ролей пользователей
CREATE TABLE USER_ROLE (
    user_role INT PRIMARY KEY,
    name VARCHAR(255) NOT NULL
);

-- 2. Справочник статусов заказов
CREATE TABLE order_status (
    status_id INT PRIMARY KEY,
    name VARCHAR(255) NOT NULL
);

-- 3. Таблица пользователей
CREATE TABLE USER_data (
    user_id INT PRIMARY KEY,
    username VARCHAR(255) NOT NULL,
    password_hash INT NOT NULL,
    user_role INT,
    FOREIGN KEY (user_role) REFERENCES USER_ROLE(user_role) ON DELETE SET NULL
);

-- 4. Таблица самих заказов (или позиций меню)
CREATE TABLE "order" (
    order_id INT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    price DOUBLE PRECISION NOT NULL
);

-- 5. Связующая таблица (Корзина / Меню заказов)
CREATE TABLE menu (
    menu_id INT PRIMARY KEY,
    user_id INT,
    order_id INT,
    order_status INT,
    FOREIGN KEY (user_id) REFERENCES USER_data(user_id) ON DELETE CASCADE,
    FOREIGN KEY (order_id) REFERENCES "order"(order_id) ON DELETE CASCADE,
    FOREIGN KEY (order_status) REFERENCES order_status(status_id) ON DELETE SET NULL
);