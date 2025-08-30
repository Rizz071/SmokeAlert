package main

import (
	"database/sql"
	"log"

	_ "github.com/jackc/pgx/v5/stdlib" // драйвер PostgreSQL для database/sql
)

func main() {

	dsn := "postgres://smoke_admin:smoke_admin1324567890@localhost:5432/smokealert_1"

	// Подключаемся к БД
	db, err := sql.Open("pgx", dsn)
	if err != nil {
		log.Fatalf("failed to open database: %v", err)
	}
	defer db.Close()

	// Проверяем соединение
	if err := db.Ping(); err != nil {
		log.Fatalf("failed to ping database: %v", err)
	}

	sqlCommands := map[string]string{
		"users": `
        CREATE TABLE IF NOT EXISTS users (
            id SERIAL PRIMARY KEY,
            login TEXT NOT NULL,
            password TEXT NOT NULL,
            name TEXT
        );`,

		"gateways": `
        CREATE TABLE IF NOT EXISTS gateways (
            id SERIAL PRIMARY KEY,
            gateway_ID_p1 INT,
            gateway_ID_p2 INT,
            gateway_ID_p3 INT,
            user_id INT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
            description TEXT NOT NULL
        );`,

		"sensors": `
        CREATE TABLE IF NOT EXISTS sensors (
            id SERIAL PRIMARY KEY,
            sensor_ID_p1 INT,
            sensor_ID_p2 INT,
            sensor_ID_p3 INT,
			last_battery_level INT,
            data REAL,
            gateway_id INT NOT NULL REFERENCES gateways(id) ON DELETE CASCADE,
            description TEXT,
            last_access_time TIME
        );`,
	}

	for name, sqlCommand := range sqlCommands {

		if _, err := db.Exec(sqlCommand); err != nil {
			log.Fatalf("failed to create table %s: %v", name, err)
		}
		log.Printf("Table %s created successfully or already exist in DB", name)
	}
}
