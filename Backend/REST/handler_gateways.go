package REST

import (
	"database/sql"
	"encoding/json"
	"net/http"
)

// обработка GET и POST
func HandlerGateways(db *sql.DB) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

		var err error

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

		// Приём входящих данных от датчика.
		if r.Method == http.MethodPost {

		}
	}
}
