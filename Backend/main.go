package main

import (
	"fmt"
	"log"
	"net/http"
)

func main() {

	db, err := openDB("postgres://smoke_admin:smoke_admin1324567890@localhost:5432/smokealert_1")
	if err != nil {
		log.Fatalf("cannot connect to DB: %v", err)
	}
	defer db.Close() // гарантированное закрытие при завершении программы
	fmt.Println("DB connected successfully")

	http.HandleFunc("/sensor", handlerSensor(db))

	port := ":8084"
	log.Printf("Сервер запущен на http://localhost%s\n", port)
	log.Fatal(http.ListenAndServe(port, nil))
}
