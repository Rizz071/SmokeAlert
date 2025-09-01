-- gateways.sql
-- Создание таблицы шлюзов

CREATE TABLE IF NOT EXISTS gateways (
    ID SERIAL PRIMARY KEY,       -- Уникальный идентификатор шлюза
    HWID1 INT,                   -- Аппаратный ID части 1
    HWID2 INT,                   -- Аппаратный ID части 2
    HWID3 INT,                   -- Аппаратный ID части 3
    UserID INT NOT NULL REFERENCES users(ID) ON DELETE CASCADE,  -- Владелец
    Description TEXT NOT NULL,   -- Описание шлюза
    LastAccessTime TIME          -- Время последнего доступа
);
