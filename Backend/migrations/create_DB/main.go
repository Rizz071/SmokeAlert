package main

import (
	"SmokeAlarmBackend/DB"
	"SmokeAlarmBackend/config"
	"fmt"
	"log"

	_ "github.com/jackc/pgx/v5/stdlib" // драйвер PostgreSQL
)

func main() {

	// Подключение к существующей служебной базе "postgres"
	db, err := DB.OpenDB(
		fmt.Sprintf("postgres://postgres:%s@%s/postgres",
			config.App.DBAdminPassword,
			config.App.DBHostProduction,
		))

	if err != nil {
		log.Fatalf("cannot connect to DB: %v", err)
	}

	defer db.Close()

	_, err = db.Exec(
		fmt.Sprintf("CREATE USER %s WITH PASSWORD '%s';",
			config.App.DBUserProduction,
			config.App.DBUserPasswordProduction,
		))

	if err != nil {
		log.Fatalf("failed to create user: %v", err)
	}

	fmt.Printf("User %s created successfully\r\n",
		config.App.DBUserProduction,
	)

	_, err = db.Exec(
		fmt.Sprintf("CREATE DATABASE %s OWNER %s;",
			config.App.DBNameProduction,
			config.App.DBUserProduction,
		))

	if err != nil {
		log.Fatalf("failed to create database: %v", err)
	}

	fmt.Printf("Database %s created successfully\r\n",
		config.App.DBNameProduction,
	)
}
