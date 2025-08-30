package main

import (
	"context"
	"database/sql"
)

func searchGatewayIDBySensorID(db *sql.DB, id1, id2, id3 int) (*int, error) {
	var gatewayID int

	query := `
        SELECT gateway_id
        FROM sensors
        WHERE sensor_ID_p1 = $1 AND sensor_ID_p2 = $2 AND sensor_ID_p3 = $3
        LIMIT 1;
    `

	err := db.QueryRowContext(context.Background(), query, id1, id2, id3).Scan(&gatewayID)
	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil // gateway не найден
		}
		return nil, err // другая ошибка
	}

	return &gatewayID, nil
}
