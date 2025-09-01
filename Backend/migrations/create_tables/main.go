package main

import (
	"SmokeAlarmBackend/DB"
	"SmokeAlarmBackend/config"
	"fmt"
	"log"

	_ "github.com/jackc/pgx/v5/stdlib" // драйвер PostgreSQL для database/sql
)

func main() {

	// Подключение к существующей служебной базе "postgres"
	db, err := DB.OpenDB(
		fmt.Sprintf("postgres://%s:%s@%s/%s",
			config.App.DBUserProduction,
			config.App.DBUserPasswordProduction,
			config.App.DBHostProduction,
			config.App.DBNameProduction,
		))

	if err != nil {
		log.Fatalf("cannot connect to DB: %v", err)
	}

	defer db.Close()
	// db, err := sql.Open("pgx",
	// 	fmt.Sprintf("postgres://%s:%s@%s/%s",
	// 		config.App.DBUserProduction,
	// 		config.App.DBUserPasswordProduction,
	// 		config.App.DBHostProduction,
	// 		config.App.DBNameProduction,
	// 	))

	// if err != nil {
	// 	log.Fatalf("cannot connect to DB: %v", err)
	// }

	DB.CreateTableFromSchema(db, "DB/users_schema.sql")
	DB.CreateTableFromSchema(db, "DB/gateways_schema.sql")
	DB.CreateTableFromSchema(db, "DB/sensors_schema.sql")
}
