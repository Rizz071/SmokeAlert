package main

import (
	"database/sql"
	"fmt"
	"log"

	_ "github.com/jackc/pgx/v5/stdlib" // драйвер PostgreSQL
)

func main() {
	// Подключение к существующей служебной базе "postgres"
	dsn := "postgres://postgres:132456@localhost:5432/postgres"

	db, err := sql.Open("pgx", dsn)
	if err != nil {
		log.Fatalf("failed to connect to postgres: %v", err)
	}
	defer db.Close()

	// Создаём нового пользователя
	_, err = db.Exec(`CREATE USER smoke_admin WITH PASSWORD 'smoke_admin1324567890';`)
	if err != nil {
		log.Fatalf("failed to create user: %v", err)
	}

	// Создаём новую базу
	_, err = db.Exec(`CREATE DATABASE smokealert_1 OWNER smoke_admin;`)
	if err != nil {
		log.Fatalf("failed to create database: %v", err)
	}

	fmt.Println("Database 'smokealert_1' and user 'smoke_admin' created successfully!")
}
