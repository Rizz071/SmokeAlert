package REST

import (
	"SmokeAlarmBackend/types"
	"context"
	"database/sql"
	"fmt"
)

func InsertUser(db *sql.DB, u types.User) (*int, error) {

	query := `
		INSERT INTO users 
		(Login, Password, Name)
		VALUES ($1, $2, $3)
		RETURNING id;
	`
	var userID int
	err := db.QueryRow(
		query,
		u.Login,
		u.Password,
		u.Name,
	).Scan(&userID)

	if err != nil {
		return nil, fmt.Errorf("failed to insert user: %w", err)
	}

	return &userID, nil
}

func RemoveUserByID(db *sql.DB, ID int) error {
	query := `DELETE FROM users WHERE id = $1`

	result, err := db.Exec(query, ID)
	if err != nil {
		return fmt.Errorf("ошибка при удалении пользователя: %w", err)
	}

	rowsAffected, err := result.RowsAffected()
	if err != nil {
		return fmt.Errorf("ошибка при получении числа удалённых строк: %w", err)
	}

	if rowsAffected == 0 {
		return fmt.Errorf("пользователь с ID %d не найден", ID)
	}

	// log.Printf("Пользователь с ID %d успешно удалён\n", ID)
	return nil
}

func GetUserByID(db *sql.DB, ID int) (*types.User, error) {
	var user types.User

	query := `
        SELECT ID, Login, Password, Name
        FROM users
        WHERE ID = $1
        LIMIT 1;
    `
	err := db.QueryRowContext(context.Background(), query, ID).Scan(
		&user.ID,
		&user.Login,
		&user.Password,
		&user.Name,
	)

	if err != nil {
		if err == sql.ErrNoRows {
			return nil, nil
		}
		return nil, fmt.Errorf("query failed: %w", err)
	}

	return &user, nil
}

func GetAllUsers(db *sql.DB) ([]types.User, error) {

	query := `
        SELECT ID, Login, Password, Name
        FROM users
    `

	rows, err := db.Query(query)
	if err != nil {
		return nil, fmt.Errorf("query failed: %w", err)
	}
	defer rows.Close()

	var usersList []types.User

	var u types.User
	for rows.Next() {
		err := rows.Scan(
			&u.ID,
			&u.Login,
			&u.Password,
			&u.Name,
		)
		if err != nil {
			return nil, fmt.Errorf("scan for all users failed: %w", err)
		}
		usersList = append(usersList, u)
	}

	if err = rows.Err(); err != nil {
		return nil, fmt.Errorf("rows looping for all users failed with error: %w", err)
	}

	return usersList, nil
}
