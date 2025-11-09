package REST

import (
	"SmokeAlarmBackend/types"
	"database/sql"
	"encoding/json"
	"net/http"
	"strconv"
	"strings"
)

var db *sql.DB

func HandlerAllUsers(db_received *sql.DB) http.HandlerFunc {
	db = db_received

	return func(w http.ResponseWriter, r *http.Request) {
		switch r.Method {
		case "GET":
			getUsers(w, r)
		case "POST":
			createUser(w, r)
		default:
			http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		}
	}
}

// /users/{id}
func HandlerOneUser(db_received *sql.DB) http.HandlerFunc {
	db = db_received

	return func(w http.ResponseWriter, r *http.Request) {
		idStr := strings.TrimPrefix(r.URL.Path, "/users/")
		id, err := strconv.Atoi(idStr)
		if err != nil {
			http.Error(w, "Invalid user ID", http.StatusBadRequest)
			return
		}

		switch r.Method {
		case "GET":
			getUser(w, r, id)
		case "PUT":
			// updateUser(w, r, id)
		case "DELETE":
			deleteUser(w, r, id)
		default:
			http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		}
	}
}

func getUsers(w http.ResponseWriter, r *http.Request) {

	users, err := GetAllUsers(db)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(users)
}

func getUser(w http.ResponseWriter, r *http.Request, id int) {

	user, err := GetUserByID(db, id)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(user)
}

func createUser(w http.ResponseWriter, r *http.Request) {

	var user types.User

	err := json.NewDecoder(r.Body).Decode(&user)
	if err != nil {
		http.Error(w, "Invalid JSON payload", http.StatusBadRequest)
		return
	}

	newUserID, err := InsertUser(db, user)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	createdUser, err := GetUserByID(db, *newUserID)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusCreated)
	json.NewEncoder(w).Encode(*createdUser)
}

// func updateUser(w http.ResponseWriter, r *http.Request, id int) {
// 	var u User
// 	if err := json.NewDecoder(r.Body).Decode(&u); err != nil {
// 		http.Error(w, err.Error(), http.StatusBadRequest)
// 		return
// 	}

// 	_, err := db.Exec(
// 		"UPDATE users SET login=$1, password=$2, name=$3 WHERE id=$4",
// 		u.Login, u.Password, u.Name, id,
// 	)
// 	if err != nil {
// 		http.Error(w, err.Error(), http.StatusInternalServerError)
// 		return
// 	}
// 	w.WriteHeader(http.StatusOK)
// }

func deleteUser(w http.ResponseWriter, r *http.Request, id int) {

	err := RemoveUserByID(db, id)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	w.WriteHeader(http.StatusNoContent)
}
