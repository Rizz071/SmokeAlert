package REST

import (
	"SmokeAlarmBackend/types"
	"context"
	"database/sql"
	"fmt"
	"time"
)

// TODO InsertGateway
// TODO RemoveGatewayByID
// TODO GetGatewayByID
// TODO GetGatewayIDByHWID	DONE

func InsertGateway(db *sql.DB, g types.Gateway) (*int, error) {

	query := `
		INSERT INTO gateways 
		(HWID1, HWID2, HWID3, UserID, Description, LastAccessTime)
		VALUES ($1, $2, $3, $4, $5, $6)
		RETURNING id;
	`
	var gatewayID int
	err := db.QueryRow(
		query,
		g.HWID1,
		g.HWID2,
		g.HWID3,
		g.UserID,
		g.Description,
		time.Now(),
	).Scan(&gatewayID)

	if err != nil {
		return nil, fmt.Errorf("failed to insert gateway: %w", err)
	}

	return &gatewayID, nil
}

func GetGatewayIDBySensorHWID(db *sql.DB, HW_ID_1, HW_ID_2, HW_ID_3 int) (*int, error) {

	query := `
        SELECT gateway_id
        FROM sensors
        WHERE sensor_ID_p1 = $1 AND sensor_ID_p2 = $2 AND sensor_ID_p3 = $3
        LIMIT 1;
    `
	var gatewayID int
	err := db.QueryRowContext(context.Background(), query, HW_ID_1, HW_ID_2, HW_ID_3).Scan(&gatewayID)
	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil // gateway не найден
		}
		return nil, err // другая ошибка
	}

	return &gatewayID, nil
}
