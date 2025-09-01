package REST

import (
	"SmokeAlarmBackend/types"
	"context"
	"database/sql"
	"fmt"
)

// TODO InsertUser
// TODO RemoveUserByID
// TODO GetUserByID
// TODO GetUserByLogin
// TODO GetAllUsersByLogin

func InsertUser(db *sql.DB, g types.User) (*int, error) {

	query := `
		INSERT INTO users 
		(Login, Password, Name)
		VALUES ($1, $2, $3)
		RETURNING id;
	`
	var userID int
	err := db.QueryRow(
		query,
		g.Login,
		g.Password,
		g.Name,
	).Scan(&userID)

	if err != nil {
		return nil, fmt.Errorf("failed to insert gateway: %w", err)
	}

	return &userID, nil
}

func GetUserIDByLogin(db *sql.DB, login string) (*int, error) {

	query := `
        SELECT ID
        FROM users
        WHERE Login = $1
        LIMIT 1;
    `
	var userID int
	err := db.QueryRowContext(context.Background(), query, login).Scan(&userID)
	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil // gateway не найден
		}
		return nil, err // другая ошибка
	}

	return &userID, nil
}

// func GetGatewayIDBySensorHWID(db *sql.DB, HW_ID_1, HW_ID_2, HW_ID_3 int) (*int, error) {

// 	query := `
//         SELECT gateway_id
//         FROM sensors
//         WHERE sensor_ID_p1 = $1 AND sensor_ID_p2 = $2 AND sensor_ID_p3 = $3
//         LIMIT 1;
//     `
// 	var gatewayID int
// 	err := db.QueryRowContext(context.Background(), query, HW_ID_1, HW_ID_2, HW_ID_3).Scan(&gatewayID)
// 	if err != nil {
// 		if err == sql.ErrNoRows {
// 			return nil, nil // gateway не найден
// 		}
// 		return nil, err // другая ошибка
// 	}

// 	return &gatewayID, nil
// }
