package crud_test

import (
	"SmokeAlarmBackend/REST"
	"SmokeAlarmBackend/types"
	"fmt"
	"testing"
)

var g = types.Gateway{
	HWID1: 50,
	HWID2: 51,
	HWID3: 52,
}

func TestGatewayssTable(t *testing.T) {

	testInsertGateway(t)
	testGetGatewayByID(t)
	testGetGatewayIDBySensorHWID(t)
	testRemoveGatewayByID(t)

}

func testInsertGateway(t *testing.T) {

	_, err := REST.InsertGateway(testDB, g)

	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	fmt.Println("Тестовые данные Gateway записаны в БД:", g)
}

func testGetGatewayByID(t *testing.T) {

	newGatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	receivedGateway, err := REST.GetGatewayByID(testDB, *newGatewayID)
	if (receivedGateway.ID != *newGatewayID) || err != nil {
		t.Fatalf("GetGatewayByID failed: %v", err)
	}
}

func testGetGatewayIDBySensorHWID(t *testing.T) {

	var gateway = types.Gateway{
		HWID1:       0,
		HWID2:       10,
		HWID3:       100,
		Description: "test gateway",
	}

	var gatewayHWID = types.GatewayHWID{
		HWID1: gateway.HWID1,
		HWID2: gateway.HWID2,
		HWID3: gateway.HWID3,
	}

	newGatewayID, err := REST.InsertGateway(testDB, gateway)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	receivedGatewayID, err := REST.GetGatewayIDByHWID(
		testDB,
		gatewayHWID.HWID1,
		gatewayHWID.HWID2,
		gatewayHWID.HWID3,
	)

	if (*receivedGatewayID != *newGatewayID) || err != nil {
		t.Fatalf("GetGatewayIDByHWID() failed: %v", err)
	}
}

func testRemoveGatewayByID(t *testing.T) {

	newGatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	err = REST.RemoveGatewayByID(testDB, *newGatewayID)
	if err != nil {
		t.Fatalf("RemoveGatewayByID() failed: %v", err)
	}

	errorGateway, err := REST.GetGatewayByID(testDB, *newGatewayID)
	if errorGateway != nil && err == nil {
		t.Fatalf("GetGatewayByID() failed: %v", err)
	}
}
