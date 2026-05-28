# SISTEMA DE AUTOMATIZACIÓN DE BAJO COSTE PARA UN INVERNADERO REAL BASADO EN TECNOLOGÍAS IOT

Proyecto desarrollado como Trabajo de Fin de Grado (TFG) basado en un sistema IoT para la monitorización y automatización de un invernadero utilizando un microcontrolador ESP32.



## Descripción del proyecto

El sistema permite capturar datos ambientales en tiempo real mediante distintos sensores conectados al ESP32 y enviarlos mediante el protocolo MQTT para su visualización y control remoto.

Además, el sistema permite accionar actuadores como una bomba de agua y un ventilador para automatizar el riego y la ventilación del invernadero.



## Tecnologías utilizadas

* ESP32
* Arduino IDE
* MQTT
* Node-RED
* WiFi
* JSON



## Sensores utilizados

* DHT22 → Temperatura y humedad ambiental
* BH1750 → Intensidad lumínica
* Sensor capacitivo 2.0 → Humedad del suelo



## Actuadores

* Bomba de agua mediante relé
* Ventilador mediante relé



## Funcionamiento del sistema

1. El ESP32 se conecta a la red WiFi.
2. Se establece conexión con el broker MQTT.
3. Los sensores capturan datos ambientales.
4. Los datos se envían en formato JSON mediante MQTT.
5. Node-RED recibe y visualiza la información.
6. El sistema permite activar remotamente los actuadores.


## Estructura del proyecto

```text
/control_invernadero_esp32.cpp
/README.md
```



## Compilación y carga

1. Abrir el proyecto en Arduino IDE.
2. Instalar las librerías necesarias:

   * WiFi.h
   * PubSubClient
   * BH1750
   * DHT sensor library
3. Seleccionar la placa ESP32.
4. Configurar SSID y contraseña WiFi.
5. Compilar y cargar el programa al ESP32.



## Proyecto académico

Este repositorio forma parte de un Trabajo de Fin de Grado orientado al desarrollo de soluciones IoT aplicadas a la automatización agrícola.



## Autor

Jesús Fernández Rueda

