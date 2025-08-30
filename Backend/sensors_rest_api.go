package main

import (
	"context"
	"database/sql"
	"fmt"
	"log"
	"time"
)

func searchSensorIDBY3ID(db *sql.DB, id1, id2, id3 int) (*int, error) {
	var sensorID int

	query := `
        SELECT id
        FROM sensors
        WHERE sensor_ID_p1 = $1 AND sensor_ID_p2 = $2 AND sensor_ID_p3 = $3
        LIMIT 1;
    `

	err := db.QueryRowContext(context.Background(), query, id1, id2, id3).Scan(&sensorID)
	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil // gateway не найден
		}
		return nil, err // другая ошибка
	}

	return &sensorID, nil
}

func updateSensorByID(db *sql.DB, sensorID int, newSensorData float32, lastAccessTime time.Time) error {
	query := `
        UPDATE sensors
        SET data = $1,
            last_access_time = $2
        WHERE id = $3;
    `

	// Выполняем запрос
	result, err := db.ExecContext(
		context.Background(),
		query,
		newSensorData,
		lastAccessTime,
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

func insertSensor(db *sql.DB, receivedSensorData sensorData) error {

	// Можно использовать данные
	log.Printf("Получено: %+v\n", receivedSensorData)

	// Данные для отправки в БД
	sensorID_1 := receivedSensorData.ID_1
	sensorID_2 := receivedSensorData.ID_2
	sensorID_3 := receivedSensorData.ID_3
	data := receivedSensorData.SensorData

	description := "Temperature sensor"
	lastAccess := time.Now().Format("15:04:05") // только время в формате HH:MM:SS

	gatewayID, err := searchGatewayIDBySensorID(db, sensorID_1, sensorID_2, sensorID_3)
	if err != nil {
		log.Fatalf("error getting gateway_id: %v", err)
	}

	if gatewayID != nil {
		log.Println("sensor found. modifying sensor.")

		updateSensorByID(db)
	}

	if gatewayID == nil {
		log.Println("sensor not found. adding new sensor.")

		_, err = db.Exec(
			`INSERT INTO sensors (sensor_ID_p1, sensor_ID_p2, sensor_ID_p3, data, gateway_id, description, last_access_time)
         VALUES ($1,$2,$3,$4,$5,$6)`, sensorID_1, sensorID_2, sensorID_3, data, gatewayID, description, lastAccess)

		if err != nil {
			log.Fatalf("failed to add sensor: %v", err)
		}
	}

	return nil
}
