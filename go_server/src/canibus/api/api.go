// Package API implements the API used by the CANiBUS Server
package api

import (
	"fmt"
)

type CanibusAPIVersion struct {
	Major int
	Minor int
	Sub   int
}

func (c *CanibusAPIVersion) ToString() string {
	return fmt.Sprintf("%d.%d.%d", c.Major, c.Minor, c.Sub)
}

type Server struct {
	Version string
}

// Cmd Structure of client issuing commands to server
type Cmd struct {
	Action string
	Arg    []string
}

type Client struct {
	ClientID int
	Cookie   string
}

type Msg struct {
	Type     string
	ClientID int
	Author   string
	Value    string
}

var APIVersion CanibusAPIVersion
var ServerVersion Server

func InitAPI() {
	APIVersion.Major = 0
	APIVersion.Minor = 0
	APIVersion.Sub = 2
	ServerVersion.Version = APIVersion.ToString()
}
