package REST

import (
	"SmokeAlarmBackend/types"
	"context"
	"database/sql"
	"fmt"
	"log"
)

// TODO InsertGateway				DONE
// TODO RemoveGatewayByID			DONE
// TODO GetGatewayByID				DONE
// TODO GetGatewayIDBySensorHWID	DONE

func InsertGateway(db *sql.DB, g types.Gateway) (*int, error) {

	query := `
		INSERT INTO gateways 
		(HWID1, HWID2, HWID3, Description)
		VALUES ($1, $2, $3, $4)
		RETURNING id;
	`
	var gatewayID int
	err := db.QueryRow(
		query,
		g.HWID1,
		g.HWID2,
		g.HWID3,
		g.Description,
	).Scan(&gatewayID)

	if err != nil {
		return nil, fmt.Errorf("failed to insert gateway: %w", err)
	}

	return &gatewayID, nil
}

func RemoveGatewayByID(db *sql.DB, ID int) error {
	query := `DELETE FROM gateways WHERE id = $1`

	result, err := db.Exec(query, ID)
	if err != nil {
		return fmt.Errorf("ошибка при удалении шлюза: %w", err)
	}

	rowsAffected, err := result.RowsAffected()
	if err != nil {
		return fmt.Errorf("ошибка при получении числа удалённых строк: %w", err)
	}

	if rowsAffected == 0 {
		return fmt.Errorf("шлюз с ID %d не найден", ID)
	}

	log.Printf("Шлюз с ID %d успешно удалён\n", ID)
	return nil
}

func GetGatewayByID(db *sql.DB, ID int) (*types.Gateway, error) {

	var gateway types.Gateway

	query := `
        SELECT ID, HWID1, HWID2, HWID3, Description, LastAccessTime
        FROM gateways
        WHERE ID = $1
        LIMIT 1;
    `
	err := db.QueryRowContext(context.Background(), query, ID).Scan(
		&gateway.ID,
		&gateway.HWID1,
		&gateway.HWID2,
		&gateway.HWID3,
		&gateway.Description,
		&gateway.LastAccessTime,
	)

	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil
		}
		return nil, fmt.Errorf("query failed: %w", err)
	}

	return &gateway, nil
}

func GetGatewayIDByHWID(db *sql.DB, HWID1, HWID2, HWID3 int) (*int, error) {
	var gatewayID int

	query := `
        SELECT ID
        FROM gateways
        WHERE HWID1 = $1 AND HWID2 = $2 AND HWID3 = $3
        LIMIT 1;
    `

	err := db.QueryRowContext(context.Background(), query, HWID1, HWID2, HWID3).Scan(&gatewayID)
	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil // gateway не найден
		}
		return nil, err // другая ошибка
	}

	return &gatewayID, nil
}
