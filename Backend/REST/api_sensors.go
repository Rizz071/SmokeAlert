package REST

import (
	"SmokeAlarmBackend/types"
	"context"
	"database/sql"
	"fmt"
	"log"
	"time"
)

func InsertSensor(db *sql.DB, s types.Sensor) (int, error) {

	query := `
		INSERT INTO sensors 
		(HWID1, HWID2, HWID3, LastBatteryLevel, Data, GatewayID, Description)
		VALUES ($1, $2, $3, $4, $5, $6, $7)
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
	).Scan(&ID)

	if err != nil {
		return 0, fmt.Errorf("failed to insert sensor: %w", err)
	}

	return ID, nil
}

func RemoveSensorByID(db *sql.DB, ID int) error {
	query := `DELETE FROM sensors WHERE id = $1`

	result, err := db.Exec(query, ID)
	if err != nil {
		return fmt.Errorf("ошибка при удалении сенсора: %w", err)
	}

	rowsAffected, err := result.RowsAffected()
	if err != nil {
		return fmt.Errorf("ошибка при получении числа удалённых строк: %w", err)
	}

	if rowsAffected == 0 {
		return fmt.Errorf("сенсор с ID %d не найден", ID)
	}

	log.Printf("Сенсор с ID %d успешно удалён\n", ID)
	return nil
}

func GetSensorByID(db *sql.DB, ID int) (*types.Sensor, error) {
	var sensor types.Sensor

	query := `
        SELECT ID, HWID1, HWID2, HWID3, LastBatteryLevel, Data, GatewayID, Description, LastAccessTime
        FROM sensors
        WHERE ID = $1
        LIMIT 1;
    `
	err := db.QueryRowContext(context.Background(), query, ID).Scan(
		&sensor.ID,
		&sensor.HWID1,
		&sensor.HWID2,
		&sensor.HWID3,
		&sensor.LastBatteryLevel,
		&sensor.Data,
		&sensor.GatewayID,
		&sensor.Description,
		&sensor.LastAccessTime,
	)

	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil
		}
		return nil, fmt.Errorf("query failed: %w", err)
	}

	return &sensor, nil
}

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
			return nil, nil // сенсор не найден
		}
		return nil, err // другая ошибка
	}

	return &sensorID, nil
}

// Обновление полей: Data, Battery, LastAcessTime
func UpdateSensorDataAndBattery(db *sql.DB, s types.SensorReceivedPacket) error {

	// Получаем ID датчика в БД
	sensorID, err := GetSensorIDByHWID(db, s.HWID1, s.HWID2, s.HWID3)
	if err != nil {
		// TODO error
	}
	if sensorID == nil {
		// TODO датчик не найден => SensorInsert()
	}

	// Формируем запрос в БД
	query := `
        UPDATE sensors
        SET LastBatteryLevel = $1, Data = $2, LastAccessTime = $3
        WHERE ID = $4;
    `

	// Выполняем запрос
	result, err := db.ExecContext(
		context.Background(),
		query,
		s.BatteryLevel,
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

func GetAllSensorsByGatewayID(db *sql.DB, gatewayID int) ([]types.Sensor, error) {

	query := `
        SELECT ID, HWID1, HWID2, HWID3, LastBatteryLevel, Data, GatewayID, Description, LastAccessTime
        FROM sensors
        WHERE GatewayID = $1;
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
