-- sensors.sql
-- Создание таблицы сенсоров

CREATE TABLE IF NOT EXISTS sensors (
    ID SERIAL PRIMARY KEY,                                              -- Уникальный идентификатор сенсора
    HWID1 INT,                                                          -- Аппаратный ID части 1
    HWID2 INT,                                                          -- Аппаратный ID части 2
    HWID3 INT,                                                          -- Аппаратный ID части 3
    LastBatteryLevel INT,                                               -- Уровень батареи
    Data REAL,                                                          -- Данные сенсора
    GatewayID INT NOT NULL REFERENCES gateways(ID) ON DELETE CASCADE,   -- Привязка к шлюзу
    Description TEXT,                                                   -- Описание сенсора
    LastAccessTime TIMESTAMPTZ DEFAULT now()                            -- Время последнего доступа
);
