package crud_test

import (
	"SmokeAlarmBackend/REST"
	"SmokeAlarmBackend/types"
	"testing"
)

func TestInsertUser(t *testing.T) {

	u := generateUser()

	_, err := REST.InsertUser(testDB, u)
	if err != nil {
		t.Fatalf("InsertUser() failed: %v", err)
	}
}

func TestRemoveUserByID(t *testing.T) {

	u := generateUser()

	newUserID, err := REST.InsertUser(testDB, u)
	if err != nil {
		t.Fatalf("InsertUser() failed: %v", err)
	}

	err = REST.RemoveUserByID(testDB, *newUserID)
	if err != nil {
		t.Fatalf("RemoveUserByID() failed: %v", err)
	}

	errorUser, err := REST.GetUserByID(testDB, *newUserID)
	if errorUser != nil && err == nil {
		t.Fatalf("RemoveUserByID() failed: %v", err)
	}
}

func TestGetUserByID(t *testing.T) {

	u := generateUser()

	newUserID, err := REST.InsertUser(testDB, u)
	if err != nil {
		t.Fatalf("InsertUser() failed: %v", err)
	}

	receivedUser, err := REST.GetUserByID(testDB, *newUserID)
	if (receivedUser.ID != *newUserID) || err != nil {
		t.Fatalf("GetUserByID() failed: %v", err)
	}
}

func TestGetAllUsers(t *testing.T) {

	var amountOfTestUsers = 10
	testPhrase := "TestGetAllUser"

	for i := 0; i < amountOfTestUsers; i++ {
		u := generateUser()
		u.Login = testPhrase
		_, err := REST.InsertUser(testDB, u)
		if err != nil {
			t.Fatalf("InsertUser() failed at step %v : %v", i, err)
		}
	}

	receivedUsers, err := REST.GetAllUsers(testDB)
	if receivedUsers == nil {
		t.Fatalf("GetAllUsers() failed : %v", err)
	}
	if err != nil {
		t.Fatalf("GetAllUsers() failed : %v", err)
	}

	var finalUsersList []types.User
	for _, user := range receivedUsers {
		if user.Login == testPhrase {
			finalUsersList = append(finalUsersList, user)
		}
	}

	if len(finalUsersList) != amountOfTestUsers {
		t.Fatalf("GetAllUsers() failed: %v", err)
	}
}
