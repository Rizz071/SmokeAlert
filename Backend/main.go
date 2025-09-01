package main

import (
	"SmokeAlarmBackend/DB"
	"SmokeAlarmBackend/REST"
	"SmokeAlarmBackend/config"
	"fmt"
	"log"
	"net/http"
)

func main() {

	// var CurrentUser types.User
	// CurrentUser.ID = 1
	// CurrentUser.Login = "DummmyLogin"
	// CurrentUser.Name = "Some user name"

	// db, err := DB.OpenDB("postgres://smoke_admin:smoke_admin1324567890@localhost:5432/smokealert_1")
	db, err := DB.OpenDB(
		fmt.Sprintf("postgres://%s:%s@%s/%s",
			config.App.DBAdmin,
			config.App.DBAdminPassword,
			config.App.DBHostProduction,
			config.App.DBNameProduction,
		))
	if err != nil {
		log.Fatalf("cannot connect to DB: %v", err)
	}
	defer db.Close() // гарантированное закрытие при завершении программы
	fmt.Println("DB connected successfully")

	http.HandleFunc("/gateways", REST.HandlerGateways(db))
	http.HandleFunc("/sensors", REST.HandlerSensors(db))

	port := ":8084"
	log.Printf("Сервер запущен на http://localhost%s\n", port)
	log.Fatal(http.ListenAndServe(port, nil))
}
