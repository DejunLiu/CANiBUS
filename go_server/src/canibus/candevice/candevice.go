// Base CanDevice Interface
package candevice

import (
)

type CanDevice interface {
	Init() bool
	DeviceType() string
	DeviceDesc() string
}

