package REST

import (
	"SmokeAlarmBackend/types"
	"context"
	"database/sql"
	"fmt"
	"time"
)

// TODO AddSensor
// TODO RemoveSensorByID
// TODO Set description
// TODO GetSensorsByGatewayID

func GetSensorIDByHWID(db *sql.DB, HWID1, HWID2, HWID3 int) (*int, error) {
	var sensorID int

	query := `
        SELECT ID
        FROM sensors
        WHERE HWID1 = $1 AND HWID2 = $2 AND HWID3 = $3
        LIMIT 1;
    `

	err := db.QueryRowContext(context.Background(), query, HWID1, HWID2, HWID3).Scan(&sensorID)
	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil // gateway не найден
		}
		return nil, err // другая ошибка
	}

	return &sensorID, nil
}

func InsertSensor(db *sql.DB, s types.Sensor) (int, error) {
	query := `
		INSERT INTO sensors 
		(HWID1, HWID2, HWID3, LastBatteryLevel, Data, GatewayID, Description, LastAccessTime)
		VALUES ($1, $2, $3, $4, $5, $6, $7, $8)
		RETURNING id;
	`

	var ID int
	err := db.QueryRow(
		query,
		s.HWID1,
		s.HWID2,
		s.HWID3,
		s.LastBatteryLevel,
		s.Data,
		s.GatewayID,
		s.Description,
		s.LastAccessTime,
	).Scan(&ID)

	if err != nil {
		return 0, fmt.Errorf("failed to insert sensor: %w", err)
	}

	return ID, nil
}

func UpdateSensor(db *sql.DB, sensorID int, s types.SensorReceivedData) error {
	query := `
        UPDATE sensors
        SET data = $1, last_access_time = $2
        WHERE id = $3;
    `

	// Выполняем запрос
	result, err := db.ExecContext(
		context.Background(),
		query,
		s.SensorData,
		time.Now(),
		sensorID,
	)
	if err != nil {
		return fmt.Errorf("failed to update sensor: %w", err)
	}

	// Проверяем, что была обновлена хотя бы одна строка
	rowsAffected, err := result.RowsAffected()
	if err != nil {
		return fmt.Errorf("failed to get affected rows: %w", err)
	}
	if rowsAffected == 0 {
		return fmt.Errorf("sensor with ID %d not found", sensorID)
	}

	return nil
}

func GetSensorsByGatewayID(db *sql.DB, gatewayID int) ([]types.Sensor, error) {

	query := `
        SELECT id, HW_ID_p1, HW_ID_p2, HW_ID_p3, last_battery_level, data, gateway_id, description, last_access_time
        FROM sensors
        WHERE gateway_id = $1;
    `

	rows, err := db.Query(query, gatewayID)
	if err != nil {
		return nil, fmt.Errorf("query failed: %w", err)
	}
	defer rows.Close()

	var sensorsList []types.Sensor

	var s types.Sensor
	for rows.Next() {
		err := rows.Scan(
			&s.ID,
			&s.HWID1,
			&s.HWID2,
			&s.HWID3,
			&s.LastBatteryLevel,
			&s.Data,
			&s.GatewayID,
			&s.Description,
			&s.LastAccessTime,
		)
		if err != nil {
			return nil, fmt.Errorf("scan for sensors by gateway ID failed: %w", err)
		}
		sensorsList = append(sensorsList, s)
	}

	if err = rows.Err(); err != nil {
		return nil, fmt.Errorf("rows looping for sensors by gateway ID failed with error: %w", err)
	}

	return sensorsList, nil
}

// func InsertSensor(db *sql.DB, receivedSensorData types.SensorReceivedData) error {

// 	// Можно использовать данные
// 	log.Printf("Получено: %+v\n", receivedSensorData)

// 	// Данные для отправки в БД
// 	sensorHWID1 := receivedSensorData.HWID1
// 	sensorHWID2 := receivedSensorData.HWID2
// 	sensorHWID3 := receivedSensorData.HWID3
// 	data := receivedSensorData.SensorData

// 	description := "Temperature sensor"
// 	lastAccessTime := time.Now().Format("15:04:05") // только время в формате HH:MM:SS

// 	gatewayID, err := GetGatewayIDBySensorHWID(db, sensorHWID1, sensorHWID2, sensorHWID3)
// 	if err != nil {
// 		log.Fatalf("error getting gateway_id: %v", err)
// 	}

// 	if gatewayID != nil {
// 		log.Println("sensor found. modifying sensor.")

// 		// UpdateSensorByID(db)
// 	}

// 	if gatewayID == nil {
// 		log.Println("sensor not found. adding new sensor.")

// 		_, err = db.Exec(
// 			`INSERT INTO sensors (sensor_ID_p1, sensor_ID_p2, sensor_ID_p3, data, gateway_id, description, last_access_time)
//          VALUES ($1,$2,$3,$4,$5,$6)`, sensorHWID1, sensorHWID2, sensorHWID3, data, gatewayID, description, lastAccessTime)

// 		if err != nil {
// 			log.Fatalf("failed to add sensor: %v", err)
// 		}
// 	}

// 	return nil
// }
