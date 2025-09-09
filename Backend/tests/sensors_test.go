package crud_test

import (
	"SmokeAlarmBackend/REST"
	"SmokeAlarmBackend/types"
	"testing"
	"time"
)

func TestInsertSensor(t *testing.T) {

	g := generateGateway()
	s := generateSensor()

	gatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	s.GatewayID = *gatewayID
	s.LastAccessTime = time.Now()

	_, err = REST.InsertSensor(testDB, s)
	if err != nil {
		t.Fatalf("InsertSensor() failed: %v", err)
	}
}

func TestRemoveSensorByID(t *testing.T) {

	g := generateGateway()

	gatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	s := generateSensor()
	s.GatewayID = *gatewayID
	s.LastAccessTime = time.Now()

	sensorID, err := REST.InsertSensor(testDB, s)
	if err != nil {
		t.Fatalf("InsertSensor() failed: %v", err)
	}

	err = REST.RemoveSensorByID(testDB, sensorID)
	if err != nil {
		t.Fatalf("RemoveSensorByID() failed: %v", err)
	}

	errorSensor, err := REST.GetSensorByID(testDB, sensorID)
	if errorSensor != nil && err == nil {
		t.Fatalf("GetSensorByID() failed: %v", err)
	}
}

func TestGetSensorByID(t *testing.T) {

	g := generateGateway()

	gatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}
	t.Log("Записан тестовый шлюз")

	s := generateSensor()
	s.GatewayID = *gatewayID
	s.LastAccessTime = time.Now()

	sensorID, err := REST.InsertSensor(testDB, s)
	if err != nil {
		t.Fatalf("InsertSensor() failed: %v", err)
	}
	t.Log("Записан тестовый сенсор")

	receivedSensor, err := REST.GetSensorByID(testDB, sensorID)
	if receivedSensor == nil {
		t.Fatal("ERROR: receivedSensor == nil")
		t.Fatalf("GetSensorByID() failed: %v", err)
	}
	if err != nil {
		t.Fatalf("GetSensorByID() failed: %v", err)
	}

	if receivedSensor.ID != sensorID {
		t.Fatalf("GetSensorByID() failed: %v", err)
	}
}

func TestGetSensorIDByHWID(t *testing.T) {

	g := generateGateway()

	gatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	s := generateSensor()
	s.GatewayID = *gatewayID
	s.LastAccessTime = time.Now()

	sensorID, err := REST.InsertSensor(testDB, s)
	if err != nil {
		t.Fatalf("InsertSensor() failed: %v", err)
	}

	receivedSensorID, err := REST.GetSensorIDByHWID(
		testDB,
		s.HWID1,
		s.HWID2,
		s.HWID3,
	)

	if (*receivedSensorID != sensorID) || err != nil {
		t.Fatalf("GetSensorIDByHWID() failed: %v", err)
	}
}

func TestUpdateSensorDataAndBattery(t *testing.T) {

	g := generateGateway()

	gatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	s := generateSensor()
	s.GatewayID = *gatewayID
	s.LastAccessTime = time.Now()

	sensorID, err := REST.InsertSensor(testDB, s)
	if err != nil {
		t.Fatalf("InsertSensor() failed: %v", err)
	}

	var newSensorData = types.SensorReceivedPacket{
		HWID1:        s.HWID1,
		HWID2:        s.HWID2,
		HWID3:        s.HWID3,
		SensorData:   42,
		BatteryLevel: 56,
	}

	err = REST.UpdateSensorDataAndBattery(testDB, newSensorData)
	if err != nil {
		t.Fatalf("UpdateSensorDataAndBattery() failed: %v", err)
	}

	receivedSensor, err := REST.GetSensorByID(testDB, sensorID)
	if err != nil {
		t.Fatalf("GetSensorByID() failed: %v", err)
	}

	if receivedSensor.Data != newSensorData.SensorData {
		t.Fatalf("UpdateSensorDataAndBattery() failed: %v", err)
	}

	if receivedSensor.LastBatteryLevel != newSensorData.BatteryLevel {
		t.Fatalf("UpdateSensorDataField() failed: %v", err)
	}
}

func TestGetAllSensorsByGatewayID(t *testing.T) {

	g := generateGateway()

	gatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	s := generateSensor()
	s.GatewayID = *gatewayID
	s.LastAccessTime = time.Now()

	var amountOfTestSensors = 10

	for i := 0; i < amountOfTestSensors; i++ {
		_, err := REST.InsertSensor(testDB, s)
		if err != nil {
			t.Fatalf("InsertSensor() failed at step %v : %v", i, err)
		}
	}

	receivedSensors, err := REST.GetAllSensorsByGatewayID(testDB, *gatewayID)
	if err != nil {
		t.Fatalf("GetAllSensorsByGatewayID() failed : %v", err)
	}

	if len(receivedSensors) != amountOfTestSensors {
		t.Fatalf("GetAllSensorsByGatewayID() failed: %v", err)
	}
}
