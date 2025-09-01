package main

import (
	"database/sql"
	"fmt"
	"log"

	_ "github.com/jackc/pgx/v5/stdlib" // драйвер PostgreSQL
)

func main() {
	adminDB, err := sql.Open("pgx", "postgres://postgres:132456@localhost:5432/postgres")
	if err != nil {
		log.Fatal(err)
	}
	defer adminDB.Close()

	_, err = adminDB.Exec(`SELECT pg_terminate_backend(pid)
                            FROM pg_stat_activity
                            WHERE datname = 'smokealert_1';`)
	if err != nil {
		log.Fatal(err)
	}

	_, err = adminDB.Exec("DROP DATABASE IF EXISTS smokealert_1;")
	if err != nil {
		log.Fatal(err)
	}

	_, err = adminDB.Exec("DROP ROLE IF EXISTS smoke_admin;")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("База и админ удалены")
}
