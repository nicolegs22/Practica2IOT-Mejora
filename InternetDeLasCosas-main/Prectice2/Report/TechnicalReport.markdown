# Práctica 2: Sistema IoT con Sensor y Actuador

**Integrante**

- Nicole Gomez

## 1. Requerimientos Funcionales y No Funcionales

### 1.1 Requerimientos funcionales

- El sistema debe conectar un sensor ESP32, un actuador ESP32 y un servidor mediante una red WiFi local.
- El sensor debe medir la distancia de un objeto usando un sensor ultrasónico.
- El sensor debe enviar la distancia medida al servidor mediante comunicación TCP.
- El servidor debe recibir los datos del sensor y procesarlos según rangos de distancia.
- El servidor debe enviar comandos al actuador para controlar las salidas conectadas a LEDs indicadores.
- El actuador debe activar el LED indicador correspondiente al rango de distancia recibido.
- El sistema debe permitir el registro de dispositivos como `sensor` o `actuator`.

### 1.2 Requerimientos no funcionales

- La comunicación debe realizarse sobre TCP para asegurar entrega ordenada de mensajes.
- Los mensajes deben enviarse en formato JSON y separarse con salto de línea (`\n`).
- El sensor debe enviar mediciones cada 1 segundo para evitar saturación.
- El servidor debe aceptar múltiples clientes conectados.
- El sistema debe funcionar dentro de una red WiFi local con conectividad entre los dispositivos.
- El código debe mantener nombres y mensajes consistentes, evitando mezcla innecesaria de idiomas.
- El sistema debe intentar reconectarse cuando un ESP32 pierda conexión con el servidor.

## 2. Diseño del Sistema

El sistema usa una arquitectura cliente-servidor. El servidor centraliza la lógica de decisión; el sensor solo mide y envía datos, mientras que el actuador recibe comandos y ejecuta la acción sobre los LEDs indicadores conectados a sus salidas.

| Componente | Función principal |
|---|---|
| Sensor ESP32 | Mide distancia y envía datos al servidor. |
| Servidor TCP | Registra clientes, procesa distancias y genera comandos. |
| Actuador ESP32 | Recibe comandos y controla las salidas de los LEDs indicadores. |

### 2.1 Diagrama de bloques

![Diagrama de bloques](Diagrams/DiagamaBloques.jpeg)

*Figura 1. Relación general entre sensor, servidor y actuador.*

### 2.2 Diagrama de circuito

**Circuito del sensor**

![Circuito del sensor](Diagrams/CircuitoSensor.jpeg)

*Figura 2. Conexión del sensor ultrasónico con el ESP32.*

**Circuito del actuador**

![Circuito del actuador](Diagrams/circuitoActuador.jpeg)

*Figura 3. Conexión del ESP32 actuador con las salidas de LED.*

### 2.3 Diagrama de arquitectura del sistema

![Diagrama de arquitectura del sistema](<Diagrams/diagrama Arquitectura del sistema.png>)

*Figura 4. Arquitectura cliente-servidor utilizada en la práctica.*

### 2.4 Especificación del protocolo de aplicación

El protocolo de aplicación usa mensajes JSON sobre TCP. Cada mensaje termina con `\n` para que el servidor pueda separarlos por unidad de mensaje.

**Puerto de comunicación**

| Parámetro | Valor |
|---|---|
| Protocolo de transporte | TCP |
| Host del servidor | `0.0.0.0` |
| Puerto | `5000` |
| Formato de mensaje | JSON |
| Separador | Salto de línea (`\n`) |

**Registro de dispositivo**

```json
{
  "message_type": "register",
  "id": "ESP32_SENSOR_01",
  "device_type": "sensor"
}
```

**Envío de datos del sensor**

```json
{
  "message_type": "sensor_data",
  "id": "ESP32_SENSOR_01",
  "distance": 15.2
}
```

**Comando hacia el actuador**

```json
{
  "message_type": "command",
  "command": "leds",
  "rgb": [0, 0, 255],
  "duration": 500
}
```

El campo `rgb` se mantiene como parte del protocolo del código fuente. En este reporte representa los canales de salida rojo, verde y azul usados para activar LEDs indicadores.

**Confirmación del actuador**

```json
{
  "message_type": "command_response",
  "id": "ESP32_ACTUATOR_01",
  "command": "leds",
  "result": "ok"
}
```

El servidor conserva compatibilidad con claves anteriores en español, como `tipo`, `dispositivo_tipo`, `distancia`, `comando`, `duracion` y `resultado`.

### 2.5 Diagramas estructurales y de comportamiento

**Diagrama de clases**

![Diagrama de clases](Diagrams/DiagramaClases.png)

*Figura 5. Estructura principal de clases del servidor y los dispositivos.*

**Diagrama de comportamiento**

![Diagrama de comportamiento](<Diagrams/Diagrama comportamiento.jpeg>)

*Figura 6. Secuencia general de medición, procesamiento y actuación.*

**Diagrama de comportamiento del protocolo**

![Diagrama de comportamiento del protocolo](<Diagrams/diagrama de comportamiento del protocolo.png>)

*Figura 7. Flujo de mensajes del protocolo de aplicación.*

## 3. Implementación

### 3.1 Servidor TCP en Python

El servidor se encuentra en `Prectice2/SourceCode/server`.

| Archivo | Responsabilidad |
|---|---|
| `main.py` | Punto de entrada para ejecutar el servidor TCP. |
| `server.py` | Acepta conexiones, procesa mensajes y envía comandos. |
| `client_manager.py` | Administra sensores y actuadores conectados. |
| `protocol.py` | Define claves, tipos de mensaje y comandos JSON. |
| `run_tcp_server.bat` | Ejecuta el servidor desde Windows. |

Para ejecutar el servidor:

```powershell
python main.py --host 0.0.0.0 --port 5000
```

### 3.2 Código fuente documentado

El código está organizado por responsabilidad:

- `IoTServer`: inicia el socket TCP, acepta clientes y procesa mensajes.
- `ClientManager`: registra, elimina y filtra clientes conectados.
- `Protocol`: centraliza el formato JSON del protocolo de aplicación.
- `Sensor`: conecta a WiFi, mide distancia y envía datos al servidor.
- `Actuator`: conecta a WiFi, recibe comandos y controla las salidas de los LEDs indicadores.

### 3.3 Sensor ESP32

El sensor se encuentra en `Prectice2/SourceCode/esp32scripts/Sensor`.

Funciones principales:

- Conectarse a la red WiFi.
- Conectarse al servidor TCP.
- Medir distancia con el sensor ultrasónico.
- Enviar la distancia al servidor cada 1 segundo.

Fórmula utilizada:

```text
distancia = duración * 0.034 / 2
```

### 3.4 Actuador ESP32

El actuador se encuentra en `Prectice2/SourceCode/esp32scripts/Actuator`.

Funciones principales:

- Conectarse a la red WiFi.
- Registrarse en el servidor como actuador.
- Recibir comandos JSON.
- Activar las salidas de LED según los valores recibidos en el campo `rgb`.
- Enviar confirmación al servidor.

## 4. Pruebas y Validaciones

### 4.1 Prueba de funcionamiento por rangos

| Distancia medida | Acción esperada |
|---:|---|
| Menor a 10 cm | LED rojo |
| 10 cm a 20 cm | LED azul |
| Mayor a 20 cm y hasta 30 cm | LED verde |
| Mayor a 30 cm | LED apagado |

Evidencias:

![Prueba LED rojo](Test/PruebaRojo.jpeg)

*Figura 8. Activación del indicador rojo para distancia menor a 10 cm.*

![Prueba LED azul](Test/PruebaAzul.jpeg)

*Figura 9. Activación del indicador azul para distancia entre 10 cm y 20 cm.*

![Prueba LED verde](Test/PruebaVerde.jpeg)

*Figura 10. Activación del indicador verde para distancia mayor a 20 cm y hasta 30 cm.*

### 4.2 Integridad de mensajes

Se verificó que los mensajes JSON enviados entre sensor, servidor y actuador lleguen completos durante la comunicación TCP.

Resultado:

- No se detectaron mensajes incompletos.
- Los datos recibidos conservaron el formato esperado.
- TCP permitió mantener orden e integridad en la transmisión.

Evidencias:

![Integridad de mensajes 1](<Test/integridad_de _los_mensajes_enviados1.jpeg>)

*Figura 11. Captura de mensajes usados para revisar integridad de transmisión.*

![Integridad de mensajes 2](<Test/integridad_de _los_mensajes_enviados2.jpeg>)

*Figura 12. Continuación de la revisión de mensajes transmitidos.*

### 4.3 Velocidad de envío de paquetes

| N | Diferencia (s) |
|---:|---:|
| 1 | 0.79 |
| 2 | 0.60 |
| 3 | 0.66 |
| 4 | 0.77 |
| 5 | 0.80 |
| 6 | 0.81 |
| 7 | 0.81 |
| 8 | 0.82 |
| 9 | 0.91 |
| 10 | 0.85 |

Promedio aproximado:

```text
0.782 s
```

Evidencias:

![Velocidad de envío de mensajes](Test/VelocidadEnvioDeMensajes.jpg)

*Figura 13. Registro usado para calcular tiempos de envío.*

![Diferencia entre entrega de paquetes](Test/diferencia_entre_entrega_paquetes.jpg)

*Figura 14. Diferencias medidas entre entregas de paquetes.*

### 4.4 Prueba de uso prolongado

Se ejecutó el sistema durante varios minutos para observar continuidad de operación.

Resultado:

- Durante los primeros minutos se registró recepción de datos y ejecución de comandos sin desconexiones reportadas en la salida del servidor.
- Después de uso prolongado se observaron errores de transmisión.
- La evidencia disponible no permite confirmar una causa única; se considera como posible causa la saturación de recursos en el ESP32.

Evidencia:

![Errores por uso prolongado](Test/Errores_debido_uso_prolongado.jpg)

*Figura 15. Evidencia de errores observados durante uso prolongado.*

## 5. Resultados

Las pruebas realizadas muestran que el servidor recibió mensajes del sensor, registró dispositivos y generó comandos para el actuador dentro de la red local.

| Rango de distancia | Salida activada | Duración |
|---|---|---:|
| `< 10 cm` | Rojo `[255, 0, 0]` | 1000 ms |
| `10 cm - 20 cm` | Azul `[0, 0, 255]` | 500 ms |
| `> 20 cm - 30 cm` | Verde `[0, 255, 0]` | 200 ms |
| `> 30 cm` | Apagado `[0, 0, 0]` | 100 ms |

Ejemplo de salida del servidor:

```text
IoT server started on 0.0.0.0:5000
[TCP] Connection from 192.168.0.20:49152
[+] Registered ESP32_ACTUATOR_01 (actuator)
[+] Registered ESP32_SENSOR_01 (sensor)
RED      |   5.2 cm
BLUE     |  15.8 cm
GREEN    |  24.3 cm
OFF      |  42.1 cm
```

En la prueba de velocidad de envío se registraron 10 mediciones con un promedio aproximado de `0.782 s`. Los valores observados estuvieron entre `0.60 s` y `0.91 s`.

No se define un umbral formal de aceptación para calificar el tiempo de respuesta; por este motivo, el resultado se reporta únicamente como dato medido.

## 6. Conclusiones

- Se verificó comunicación TCP entre sensor, servidor y actuador durante las pruebas realizadas en red local.
- El servidor recibió distancias y generó comandos de activación de LEDs según los rangos definidos en la tabla de resultados.
- Los mensajes JSON permitieron identificar campos como `message_type`, `distance`, `command`, `rgb` y `duration` durante las pruebas.
- En la prueba de uso prolongado se observaron errores de transmisión después de varios minutos, por lo que no se afirma operación continua sin errores.

## 7. Recomendaciones

- Mantener fija la IP del servidor o reservarla en el router.
- Promediar varias mediciones del sensor para reducir ruido.
- Mejorar la reconexión automática ante cortes WiFi.
- Revisar uso de memoria en el ESP32 para sesiones largas.
- Mantener el protocolo documentado antes de modificar claves JSON.

## 8. Anexos

### 8.1 Estructura del proyecto

```text
SourceCode/
├── server/
│   ├── main.py
│   ├── server.py
│   ├── protocol.py
│   ├── client_manager.py
│   └── run_tcp_server.bat
└── esp32scripts/
    ├── Sensor/
    │   ├── platformio.ini
    │   └── src/
    │       ├── main.cpp
    │       ├── sensor.cpp
    │       └── sensor.h
    └── Actuator/
        ├── platformio.ini
        └── src/
            ├── main.cpp
            ├── actuator.cpp
            └── actuator.h
```

### 8.2 Componentes utilizados

- 2 ESP32 Wemos D1 R32.
- 1 sensor ultrasónico HC-SR04.
- 3 LEDs indicadores: rojo, azul y verde.
- 3 resistencias de 220 ohm.
- Protoboards y cables de conexión.
- Computadora ejecutando el servidor TCP.

### 8.3 Conexiones principales

**Sensor HC-SR04**

| Pin HC-SR04 | Conexión ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO5 |
| ECHO | GPIO18 |

**LEDs indicadores**

| LED indicador | Conexión ESP32 |
|---|---|
| Rojo | GPIO13 con resistencia de 220 ohm |
| Verde | GPIO12 con resistencia de 220 ohm |
| Azul | GPIO14 con resistencia de 220 ohm |

### 8.4 Evidencias del montaje

![Práctica armada](imagenAnnexs/PracticaArmada.jpeg)

*Figura 16. Montaje general de la práctica.*

![ESP32 sensor](imagenAnnexs/EspSensor.jpg)

*Figura 17. Módulo ESP32 usado como sensor.*

![ESP32 actuador](imagenAnnexs/EspActuador.jpeg)

*Figura 18. Módulo ESP32 usado como actuador.*
