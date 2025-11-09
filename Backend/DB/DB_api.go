package DB

import (
	"database/sql"
	"fmt"
	"log"
	"os"

	_ "github.com/jackc/pgx/v5/stdlib"
)

// openDB открывает соединение с БД и возвращает указатель на *sql.DB
func OpenDB(dsn string) (*sql.DB, error) {

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

func CreateTableFromSchema(db *sql.DB, pathToSchema string) {

	sqlBytes, err := os.ReadFile(pathToSchema)
	if err != nil {
		log.Fatal(err)
	}
	_, err = db.Exec(string(sqlBytes))

	if err != nil {
		log.Fatalf("failed to create table: %v", err)
	}

	fmt.Println("Table", pathToSchema, "created successfully!")
}
