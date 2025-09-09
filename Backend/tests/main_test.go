package crud_test

import (
	"SmokeAlarmBackend/DB"
	"SmokeAlarmBackend/config"
	"SmokeAlarmBackend/types"
	"database/sql"
	"fmt"
	"log"
	"math/rand/v2"
	"os"
	"testing"
	"time"

	_ "github.com/jackc/pgx/v5/stdlib"
)

var testDB *sql.DB

func TestMain(m *testing.M) {

	clearTestDB()

	testDB = setupTestDB()
	defer testDB.Close()

	// Запуск всех тестов в пакете
	code := m.Run()

	clearTestDB()

	os.Exit(code)
}

func clearTestDB() {

	db, err := DB.OpenDB(
		fmt.Sprintf(`postgres://postgres:%s@%s/postgres`,
			config.App.DBAdminPassword,
			config.App.DBHostProduction,
		))

	if err != nil {
		log.Fatalf("cannot connect to DB: %v", err)
	}
	log.Println("DB postgres: opened, pinged")

	defer db.Close()

	_, err = db.Exec(`SELECT pg_terminate_backend(pid)
	                        FROM pg_stat_activity
	                        WHERE datname = 'smokealert_test';`)
	if err != nil {
		log.Fatalf("pg_terminate_backend(pid) failed: %v", err)
	}

	_, err = db.Exec("DROP DATABASE IF EXISTS smokealert_test;")
	if err != nil {
		log.Fatalf("DROP DATABASE IF EXISTS smokealert_test: %v", err)
	}

	_, err = db.Exec("DROP ROLE IF EXISTS test_smoke_admin;")
	if err != nil {
		log.Fatalf("DROP ROLE IF EXISTS test_smoke_admin failed: %v", err)
	}
}

func setupTestDB() *sql.DB {

	// Подключение к существующей служебной базе "postgres"
	db, err := DB.OpenDB(
		fmt.Sprintf("postgres://postgres:%s@%s/postgres",
			config.App.DBAdminPassword,
			config.App.DBHostProduction,
		))

	if err != nil {
		panic(fmt.Errorf("cannot connect to DB: %v", err))
	}

	defer db.Close()

	_, err = db.Exec(
		fmt.Sprintf("CREATE USER %s WITH PASSWORD '%s';",
			config.App.DBUserTesting,
			config.App.DBUserPasswordTesting,
		))

	if err != nil {
		log.Fatalf("failed to create user: %v", err)
	}

	log.Printf("User %s created successfully\r\n",
		config.App.DBUserProduction,
	)

	_, err = db.Exec(
		fmt.Sprintf("CREATE DATABASE %s OWNER %s;",
			config.App.DBNameTesting,
			config.App.DBUserTesting,
		))

	if err != nil {
		log.Fatalf("failed to create database: %v", err)
	}

	log.Printf("Database %s created successfully\r\n",
		config.App.DBNameTesting,
	)

	// testDB, err := DB.OpenDB("postgres://test_smoke_admin:test_smoke_admin@localhost:5432/smokealert_test")
	testDB, err := DB.OpenDB(
		fmt.Sprintf("postgres://%s:%s@%s/%s",
			config.App.DBUserTesting,
			config.App.DBUserPasswordTesting,
			config.App.DBHostTesting,
			config.App.DBNameTesting,
		))

	if err != nil {
		log.Fatalf("cannot connect to test DB: %v", err)
	}

	if err != nil {
		log.Fatalf("Cannot connect to test DB: %v", err)
	}

	DB.CreateTableFromSchema(testDB, "../DB/users_schema.sql")
	DB.CreateTableFromSchema(testDB, "../DB/gateways_schema.sql")
	DB.CreateTableFromSchema(testDB, "../DB/sensors_schema.sql")

	return testDB
}

func generateUser() types.User {
	return types.User{
		ID:       0, // Устанавливается автоматически при добавлении в БД
		Login:    "Test user",
		Password: "Test password",
		Name:     "Test fullname",
	}
}

func generateGateway() types.Gateway {
	return types.Gateway{
		ID:             0, // Устанавливается автоматически при добавлении в БД
		HWID1:          rand.IntN(1000000),
		HWID2:          rand.IntN(1000000),
		HWID3:          rand.IntN(1000000),
		Description:    "Test gateway",
		LastAccessTime: time.Time{}, // Установка нулевого значения
	}
}

func generateSensor() types.Sensor {
	return types.Sensor{
		ID:               0, // Устанавливается автоматически при добавлении в БД
		HWID1:            rand.IntN(1000000),
		HWID2:            rand.IntN(1000000),
		HWID3:            rand.IntN(1000000),
		LastBatteryLevel: rand.IntN(101), // В процентах
		Data:             rand.Float32() * 4,
		GatewayID:        0, // Устанавливается автоматически при добавлении в БД
		Description:      "Test sensor",
		LastAccessTime:   time.Time{}, // Установка нулевого значения
	}
}
