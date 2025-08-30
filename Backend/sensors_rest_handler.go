package main

import (
	"database/sql"
	"encoding/json"
	"log"
	"net/http"
	"time"
)

// обработка GET и POST
func handlerSensor(db *sql.DB) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

		var err error

		// Запрос на данные с одного датчика
		if r.Method == http.MethodGet {

			// Декодируем JSON в структуру
			var SensorID string
			err = json.NewDecoder(r.Body).Decode(&SensorID)
			if err != nil {
				http.Error(w, err.Error(), http.StatusBadRequest)
				return
			}

			http.Error(w, "Only POST method is allowed", http.StatusMethodNotAllowed)
			return
		}

		// Приём входящих данных от датчика.
		if r.Method == http.MethodPost {
			var receivedSensorData sensorData

			// Декодируем JSON из тела запроса
			err = json.NewDecoder(r.Body).Decode(&receivedSensorData)
			if err != nil {
				http.Error(w, err.Error(), http.StatusBadRequest)
				return
			}

			// Проверяем, существует ли sensor в БД
			sensorID, err := searchSensorIDBY3ID(db, receivedSensorData.ID_1, receivedSensorData.ID_2, receivedSensorData.ID_3)
			if err != nil {
				log.Fatalf("error in searching for sensor in DB")
			}
			if sensorID != nil {
				err = updateSensorByID(db, *sensorID, receivedSensorData.SensorData, time.Now())
				if err != nil {
					log.Fatalln("failed to update sensor data in DB")
				}
			}
			if sensorID == nil {
				err = insertSensor(db)
			}

			// Запись в БД
			w.Header().Set("Content-Type", "application/json")
			if err = insertSensor(db, receivedSensorData); err != nil {
				w.Header().Set("Content-Type", "application/json")
				w.WriteHeader(http.StatusBadRequest)
			} else {
				w.WriteHeader(http.StatusOK)
			}
		}
	}
}
