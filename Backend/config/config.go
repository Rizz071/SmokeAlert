package config

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/joho/godotenv"
)

type Config struct {
	DBAdmin                  string
	DBAdminPassword          string
	DBUserProduction         string
	DBUserPasswordProduction string
	DBUserTesting            string
	DBUserPasswordTesting    string
	DBHostProduction         string
	DBNameProduction         string
	DBHostTesting            string
	DBNameTesting            string
}

var App Config

func init() {
	// путь к .env относительно текущего исходного файла

	envPath, _ := os.Getwd()
	_, err := os.Stat(filepath.Join(envPath, ".env"))
	if os.IsNotExist(err) {
		// We are in testing
		envPath = filepath.Join(envPath, "..")
	}
	fmt.Println(filepath.Join(envPath, ".env"))
	_ = godotenv.Load(filepath.Join(envPath, ".env"))

	App = Config{
		DBAdmin:                  os.Getenv("DB_ADMIN"),
		DBAdminPassword:          os.Getenv("DB_ADMIN_PASSWORD"),
		DBUserProduction:         os.Getenv("DB_USER_PRODUCTION"),
		DBUserPasswordProduction: os.Getenv("DB_USER_PASSWORD_PRODUCTION"),
		DBUserTesting:            os.Getenv("DB_USER_TESTING"),
		DBUserPasswordTesting:    os.Getenv("DB_USER_PASSWORD_TESTING"),
		DBHostProduction:         os.Getenv("DB_HOST_PRODUCTION"),
		DBNameProduction:         os.Getenv("DB_NAME_PRODUCTION"),
		DBHostTesting:            os.Getenv("DB_HOST_TESTING"),
		DBNameTesting:            os.Getenv("DB_NAME_TESTING"),
	}
}
