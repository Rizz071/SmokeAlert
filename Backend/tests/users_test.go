package crud_test

import (
	"SmokeAlarmBackend/REST"
	"SmokeAlarmBackend/types"
	"fmt"
	"testing"
)

var u = types.User{
	Login:    "text_login",
	Password: "test_password",
	Name:     "John Doe",
}

func TestUsersTable(t *testing.T) {

	testInsertUser(t)
	testGetUserByID(t)
	testRemoveUserByID(t)
	testGetAllUsers(t)

}

func testInsertUser(t *testing.T) {

	_, err := REST.InsertUser(testDB, u)
	if err != nil {
		t.Fatalf("InsertUser() failed: %v", err)
	}

	fmt.Println("Тестовые данные User записаны в БД:", u)
}

func testGetUserByID(t *testing.T) {

	newUserID, err := REST.InsertUser(testDB, u)
	if err != nil {
		t.Fatalf("InsertUser() failed: %v", err)
	}

	receivedUser, err := REST.GetUserByID(testDB, *newUserID)
	if (receivedUser.ID != *newUserID) || err != nil {
		t.Fatalf("GetUserByID() failed: %v", err)
	}
}

func testRemoveUserByID(t *testing.T) {

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

func testGetAllUsers(t *testing.T) {

	allUsers, err := REST.GetAllUsers(testDB)
	if err != nil {
		t.Fatalf("testGetAllUsers() failed: %v", err)
	}

	fmt.Println("testGetAllUsers() returned:")
	fmt.Println(allUsers)
}
