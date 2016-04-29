# WiFi Relay 2Way Switch

The project is based on esp8266-07 or 12 and can work with [home-assistant.io](https://home-assistant.io/) via MQTT. It designed for case when you want to control e.g. a light manually by a switch or remotely from home automation server or mobile app.

![appearance](/switcher_photo1.jpg "Аppearance")

#### Key features:
- work via WiFi and reconnecting in case of a failure
- supporting of MQTT communicating and reconnecting in case of a failure
- the solution can work without WiFi and unavailability of MQTT broker
- the relay can be managed by the switch on the board or by MQTT signal


#### Circuit
![Circuit](/circuit_ss.png "Circuit")

The one was made in KiCad app, here's [source of the circuit](https://github.com/glebv/esp8266-wifi-relay-2way-switch/blob/master/circuit.sch)

### Installation
The sketch works with home-assisatnt.io, so you need:
- [Install home-assistant.io](https://home-assistant.io/getting-started/)
- [Install MQTT broker and configure it for work with HA](https://home-assistant.io/components/mqtt/)
- [Add configuration of the swicth to config file of HA ](/homeassistant/.configuration.yaml)
- [Assign your credentials to credentials.cpp](/src/credentials.cpp)
- Build and upload the code to esp8266-07 via Arduino IDE or PlatformIO

It should work! More information on my [site](http://vinnikov.net) I'll be happy to help you! 

