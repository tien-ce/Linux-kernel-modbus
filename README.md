# Modbus RTU Linux Kernel Driver (Raspberry Pi 4)

A Linux kernel driver implementing a complete **Modbus RTU master** over RS485 for Raspberry Pi 4.
The full protocol stack runs in kernel space. Each connected slave is exposed as a character device
and a sysfs node. No userspace Modbus library is needed.

License: MIT · Target: ARM64, kernel 6.12

## Source code

The source code is located in version branchs.

---
## Architecture

Two loadable kernel modules communicate through a shared callback interface:

```
User Space
┌──────────────────────────────────────────────────────────┐
│  /dev/co_sensor  /dev/pm_sensor                          │
│  /sys/class/modbusclass/<device>/...                     │
└───────────────────────────┬──────────────────────────────┘
                            │
┌───────────────────────────▼──────────────────────────────┐
│  modbus_device_module.ko                                 │
│  Platform device driver · char device · sysfs            │
└───────────────────────────┬──────────────────────────────┘
                            │  ModbusSend() / ModbusReceive()
┌───────────────────────────▼──────────────────────────────┐
│  modbus_controller_module.ko                             │
│  Serdev UART driver · Modbus RTU FSM · hrtimer (T3.5)    │
└───────────────────────────┬──────────────────────────────┘
                            │  UART2 / RS485
                     ┌──────┴──────┐
                     │  CO Sensor  │  │  PM Sensor  │
                     └─────────────┘  └─────────────┘
```



## Author

Văn Tiến — tien11102004@gmail.com
