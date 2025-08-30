package main

import (
	"database/sql"
	"fmt"
)

// openDB открывает соединение с БД и возвращает указатель на *sql.DB
func openDB(dsn string) (*sql.DB, error) {
	db, err := sql.Open("pgx", dsn)
	if err != nil {
		return nil, fmt.Errorf("failed to open database: %w", err)
	}

	// Проверяем соединение
	if err := db.Ping(); err != nil {
		db.Close() // закрываем, если пинг не удался
		return nil, fmt.Errorf("failed to ping database: %w", err)
	}

	return db, nil
}
