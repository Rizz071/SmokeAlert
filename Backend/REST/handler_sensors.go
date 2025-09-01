package REST

import (
	"SmokeAlarmBackend/types"
	"database/sql"
	"encoding/json"
	"log"
	"net/http"
)

// обработка GET и POST
func HandlerSensors(db *sql.DB) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

		var err error

		// GET
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

		// POST
		if r.Method == http.MethodPost {

			var sensorReceivedData types.SensorReceivedData

			// Декодируем JSON из тела запроса
			err = json.NewDecoder(r.Body).Decode(&sensorReceivedData)
			if err != nil {
				http.Error(w, err.Error(), http.StatusBadRequest)
				return
			}

			// Проверяем, существует ли sensor в БД
			sensorID, err := GetSensorIDByHWID(db, sensorReceivedData.HWID1, sensorReceivedData.HWID2, sensorReceivedData.HWID3)
			if err != nil {
				// TODO Error unknown
			}
			if sensorID != nil {
				// Sensor found ib DB => Update sensor
				if err = UpdateSensor(db, *sensorID, sensorReceivedData); err != nil {
					log.Fatalln("failed to update sensor data in DB")
				}
			}
			if sensorID == nil {
				// No such sensor in DB => Insert Sensor

				var newSensor types.Sensor

				newSensor.HWID1 = sensorReceivedData.HWID1
				newSensor.HWID2 = sensorReceivedData.HWID2
				newSensor.HWID3 = sensorReceivedData.HWID3
				newSensor.LastBatteryLevel = sensorReceivedData.BatteryLevel
				newSensor.Data = sensorReceivedData.SensorData
				newSensor.GatewayID = sensorReceivedData.GatewayID

				if _, err := InsertSensor(db, newSensor); err != nil {
					log.Fatalln("failed to create sensor in DB")
				}
			}

			// Запись в БД
			// w.Header().Set("Content-Type", "application/json")
			// if err = InsertSensor(db, sensorReceivedData); err != nil {
			// 	w.Header().Set("Content-Type", "application/json")
			// 	w.WriteHeader(http.StatusBadRequest)
			// } else {
			// 	w.WriteHeader(http.StatusOK)
			// }
		}
	}
}
