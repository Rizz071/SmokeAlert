-- users.sql
-- Создание таблицы пользователей

CREATE TABLE IF NOT EXISTS users (
    ID SERIAL PRIMARY KEY,        -- Уникальный идентификатор
    Login TEXT NOT NULL,          -- Логин пользователя
    Password TEXT NOT NULL,       -- Хэш пароля
    Name TEXT                     -- Имя пользователя (необязательное)
);