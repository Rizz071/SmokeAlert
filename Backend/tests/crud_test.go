package crud_test

import (
	"SmokeAlarmBackend/DB"
	"SmokeAlarmBackend/REST"
	"SmokeAlarmBackend/config"
	"SmokeAlarmBackend/types"
	"database/sql"
	"fmt"
	"log"
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

func testSensorsTable(t *testing.T, testDB *sql.DB, gatewayID int) {
	var err error

	s := types.Sensor{
		HWID1:            101,
		HWID2:            102,
		HWID3:            103,
		LastBatteryLevel: 99,
		Data:             1.1,
		GatewayID:        gatewayID,
		Description:      "Test sensor",
		LastAccessTime:   time.Now(),
	}

	// Запись нового сенсора в БД
	_, err = REST.InsertSensor(testDB, s)
	if err != nil {
		t.Fatalf("Insert failed: %v", err)
	}
	fmt.Println("Тестовые данные Sensor записаны в БД:", s)

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
	fmt.Println("DB postgres: opened, pinged")

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
	fmt.Println("DB smokealert_test: droped")

	_, err = db.Exec("DROP ROLE IF EXISTS test_smoke_admin;")
	if err != nil {
		log.Fatalf("DROP ROLE IF EXISTS test_smoke_admin failed: %v", err)
	}
	fmt.Println("USER test_smoke_admin: droped")
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

	fmt.Printf("User %s created successfully\r\n",
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

	fmt.Printf("Database %s created successfully\r\n",
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

	fmt.Println("DB connected successfully")

	if err != nil {
		log.Fatalf("Cannot connect to test DB: %v", err)
	}

	DB.CreateTableFromSchema(testDB, "../DB/users_schema.sql")
	DB.CreateTableFromSchema(testDB, "../DB/gateways_schema.sql")
	DB.CreateTableFromSchema(testDB, "../DB/sensors_schema.sql")

	return testDB
}
