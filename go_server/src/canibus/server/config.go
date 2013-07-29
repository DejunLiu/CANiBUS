package server

import (
	"canibus/candevice"
	"canibus/logger"
	"encoding/json"
	"os"
	"io"
	"fmt"
)

type ConfigElement struct {
	DeviceType string
	DeviceFile string
	DeviceSerial string
}

type Config struct {
	Drivers []candevice.CanDevice
}

func (c *Config) LoadConfig(conf string) {
	if len(conf) == 0 {
		logger.Log("No config file given")
		return
	}
	cfile, err := os.Open(conf)
	if err != nil {
		logger.Log("Could not open config file")
		return
	}
	var elem []ConfigElement
	dec := json.NewDecoder(cfile)
	for {
		err = dec.Decode(&elem)
		if err != nil && err != io.EOF {
			fmt.Println("Error:", err)
			logger.Log("Could not decode config element")
		} else {
			if err == io.EOF {
				break
			}
			for i := range elem {
				if elem[i].DeviceType == "simulator" {
					dev := &candevice.Simulator{}
					dev.SetPacketFile(elem[i].DeviceFile)
					c.Drivers = append(c.Drivers, dev)
				} else {
					fmt.Printf("Unknown config setting: %+v\n", elem[i])
				}
			}
		}
	}
	cfile.Close()
}

func (c *Config) GetDrivers() []candevice.CanDevice {
	return c.Drivers
}
