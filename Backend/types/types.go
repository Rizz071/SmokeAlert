package types

import "time"

type User struct {
	ID       int    `json:"ID"`
	Login    string `json:"Login"`
	Password string `json:"Password"`
	Name     string `json:"Name"`
}

type SensorReceivedData struct {
	HWID1        int     `json:"HWID1"`
	HWID2        int     `json:"HWID2"`
	HWID3        int     `json:"HWID3"`
	SensorData   float32 `json:"SensorData"`
	BatteryLevel float32 `json:"BatteryLevel"`
	GatewayID    int     `json:"GatewayID"`
}

type Gateway struct {
	ID          int
	HWID1       int
	HWID2       int
	HWID3       int
	Description string

	LastAccessTime time.Time
}

type Sensor struct {
	ID               int
	HWID1            int
	HWID2            int
	HWID3            int
	LastBatteryLevel float32
	Data             float32
	GatewayID        int

	Description    string
	LastAccessTime time.Time
}

type GatewayHWID struct {
	HWID1 int `json:"HWID1"`
	HWID2 int `json:"HWID2"`
	HWID3 int `json:"HWID3"`
}

type SensorHWID struct {
	HWID1 int `json:"HWID1"`
	HWID2 int `json:"HWID2"`
	HWID3 int `json:"HWID3"`
}
