package main

type sensorData struct {
	ID_1         int     `json:"ID_1"`
	ID_2         int     `json:"ID_2"`
	ID_3         int     `json:"ID_3"`
	SensorData   float32 `json:"value"`
	BatteryLevel uint8   `json:"battery_level"`
}

type sensorID struct {
	ID_1 int `json:"ID_1"`
	ID_2 int `json:"ID_2"`
	ID_3 int `json:"ID_3"`
}

type gatewayID struct {
	ID_1 int `json:"ID_1"`
	ID_2 int `json:"ID_2"`
	ID_3 int `json:"ID_3"`
}
