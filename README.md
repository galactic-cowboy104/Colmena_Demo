# README

## Descripción del Proyecto

Este proyecto consiste en dos partes: **Server** y **Client**. El **Server** está diseñado para funcionar con una cámara ESP32, permitiendo la transmisión de video en tiempo real y el control de un robot a través de una interfaz web. A continuación se detallan las funciones y características de ambos códigos.

## Server

El código del **Server** utiliza varias librerías para controlar la cámara y manejar la conexión WiFi. A continuación se describen las librerías utilizadas:

- `esp_camera.h`: Librería para controlar la cámara con el ESP32.
- `WiFi.h`: Librería para la conexión WiFi.
- `esp_timer.h`: Librería para controlar tiempos y eventos.
- `img_converters.h`: Librería para conversión de imágenes.
- `fb_gfx.h`: Librería para gráficos.
- `soc/soc.h`: Librería para configurar el sistema-on-chip.
- `soc/rtc_cntl_reg.h`: Librería para controlar registros RTC.
- `esp_http_server.h`: Librería para implementar un servidor HTTP.

### Configuración del Server

- **SSID y Contraseña**: Se definen variables para la red WiFi que se utilizará.
- **Delimitador**: Se define un delimitador de partes para la transmisión de imágenes.
- **Modelo de Cámara**: Se establece el modelo de cámara (AI-THINKER).
- **Pines de la Cámara**: Se configuran los pines de conexión de la cámara.

### Funciones Principales

- **index_handler**: Maneja la página principal que se carga en el navegador, mostrando el streaming de la cámara y los controles del robot.
- **stream_handler**: Maneja la transmisión de video desde la cámara en formato JPEG. Captura frames y los envía al cliente.
- **cmd_handler**: Maneja los comandos enviados desde el cliente para controlar el robot y el estado del flash.

### Interfaz Web

El servidor proporciona una interfaz HTML que permite a los usuarios controlar el robot mediante botones para moverse en diferentes direcciones y un interruptor para activar el flash. Se utilizan tecnologías web como HTML, CSS y JavaScript para crear una experiencia interactiva.

---

## Client

El código del **Client** es responsable de recibir comandos del usuario a través de la comunicación serie y controlar el movimiento del robot. Además, utiliza luces LED NeoPixel para proporcionar retroalimentación visual sobre el estado del sistema. A continuación se detallan las características y funciones del código del **Client**.

### Librerías Utilizadas

- `Adafruit_NeoPixel.h`: Librería para controlar los LEDs NeoPixel, que se utilizan para indicar el estado del robot.

### Definiciones de Pines

- **Pines de Motor**: Se definen cuatro pines (OUTA1, OUTA2, OUTB1, OUTB2) para controlar el movimiento de dos motores.
- **Pines de LED**: Se define un pin (PIXELS_PIN) para los NeoPixels y el número de LEDs (NUMPIXELS).

### Inicialización

- **Comunicación Serie**: Se establece la comunicación serie a 9600 baudios.
- **Inicialización de LEDs**: Los LEDs NeoPixel se inicializan y se encienden en rojo secuencialmente, indicando el inicio del sistema. Después de un segundo, se encienden en verde, señalando que el sistema está listo para recibir comandos.
- **Configuración de Pines de Motor**: Los pines para los motores se configuran como salidas.

### Bucle Principal

- **Lectura de Comandos**: En el bucle principal (`loop`), el código verifica si hay datos disponibles en el puerto serie. Si se recibe un comando, se procesa y se ejecuta la acción correspondiente. Esto se realiza mediante la función `handleCommand()`.

### Manejo de Comandos

La función `handleCommand(String cmd)` interpreta los comandos recibidos a través de la comunicación serie:

- **Comandos Soportados**:
  - `forward`: Mueve el robot hacia adelante.
  - `right`: Gira el robot a la derecha.
  - `left`: Gira el robot a la izquierda.
  - `backward`: Mueve el robot hacia atrás.
  - `stop`: Detiene el movimiento del robot.

### Funciones de Movimiento

Cada comando se asocia con una función específica que configura los pines de los motores para lograr el movimiento deseado:

- **moveForward()**: Activa los motores para avanzar.
- **moveRight()**: Activa los motores para girar a la derecha.
- **moveLeft()**: Activa los motores para girar a la izquierda.
- **moveBackward()**: Activa los motores para retroceder.
- **stopMovement()**: Detiene todos los motores.

### Resumen

El **Client** permite la interacción con el robot, enviando comandos a través de una interfaz de comunicación serie. La retroalimentación visual mediante LEDs NeoPixel proporciona una indicación clara del estado del robot, haciendo que el sistema sea intuitivo y fácil de usar. Este diseño puede ser utilizado en aplicaciones de robótica, control remoto y aprendizaje de programación de motores.

---

## Resumen del Proyecto

El proyecto combina hardware (ESP32 y cámara) con software (servidor web y control de robot) para crear una aplicación funcional que permite la visualización y control remoto. Este sistema puede ser utilizado en diversas aplicaciones, como robótica, vigilancia y telepresencia.

## Requisitos

- **Hardware**: ESP32 con cámara AI-THINKER.
- **Software**: Entorno de desarrollo para ESP32, como Arduino IDE o PlatformIO.

## Instalación

1. Conectar el ESP32 a la computadora.
2. Cargar el código del **Server** en el ESP32 utilizando el entorno de desarrollo.
3. Asegurarse de que el ESP32 esté conectado a una red WiFi.
4. Abrir el navegador y acceder a la dirección IP del ESP32 para utilizar la interfaz web.

## Contribuciones

Si deseas contribuir a este proyecto, siéntete libre de enviar un pull request o abrir un issue para discutir nuevas características.

## Licencia

Este proyecto está bajo la Licencia MIT. Puedes ver el archivo LICENSE para más detalles.
