package candevice

import (
	"canibus/hacksession"
	"canibus/logger"
	"encoding/json"
	"os"
	"io"
)

type Simulator struct {
	PacketFile string
	Packets []CanData
	HackSession *hacksession.HackSession
	id int
}

func (sim *Simulator) SetPacketFile(packets string) {
	sim.PacketFile = packets
}

func (sim *Simulator) Init() bool {
	logger.Log("Loading packets from " + sim.PacketFile)
	packets, err := os.Open(sim.PacketFile)
	if err != nil {
		logger.Log("Could not open Simulator data file")
		return false
	}
	buf := make([]byte, 1024)
	var canPacket []CanData
	for {
		n, err := packets.Read(buf)
		if err != nil && err != io.EOF {
			logger.Log("Could not read Simulator data file")
			return false
		}
		if n == 0 {
			break
		}
		err = json.Unmarshal(buf, &canPacket)
		if err != nil {
			logger.Log("Problem with json unmarshal sim data")
		} else {
			sim.Packets = append(sim.Packets, canPacket...)
		}
	}
	packets.Close()
	return true
}

func (sim *Simulator) DeviceDesc() string {
	return sim.PacketFile
}

func (sim *Simulator) DeviceType() string {
	return "Simulator"
}

func (sim *Simulator) GetHackSession() *hacksession.HackSession {
	return sim.HackSession
}

func (sim *Simulator) SetHackSession(hsession hacksession.HackSession) {
	sim.HackSession = &hsession
}

func (sim *Simulator) GetId() int {
	return sim.id
}

func (sim *Simulator) SetId(id int) {
	sim.id = id
}

