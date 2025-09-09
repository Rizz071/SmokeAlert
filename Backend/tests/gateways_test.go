package crud_test

import (
	"SmokeAlarmBackend/REST"
	"SmokeAlarmBackend/types"
	"testing"
)

func TestInsertGateway(t *testing.T) {

	g := generateGateway()

	_, err := REST.InsertGateway(testDB, g)

	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}
}

func TestRemoveGatewayByID(t *testing.T) {

	g := generateGateway()

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

func TestGetGatewayByID(t *testing.T) {

	g := generateGateway()

	newGatewayID, err := REST.InsertGateway(testDB, g)
	if err != nil {
		t.Fatalf("InsertGateway() failed: %v", err)
	}

	receivedGateway, err := REST.GetGatewayByID(testDB, *newGatewayID)
	if (receivedGateway.ID != *newGatewayID) || err != nil {
		t.Fatalf("GetGatewayByID failed: %v", err)
	}
}

func TestGetGatewayIDByHWID(t *testing.T) {

	gateway := generateGateway()

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
