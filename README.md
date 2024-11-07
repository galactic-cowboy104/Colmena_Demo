# README

## Descripción del Proyecto

Este proyecto consiste en dos componentes principales: **Server** y **Client**. El **Server** está diseñado para funcionar con una cámara ESP32, permitiendo la transmisión de video en tiempo real y el control de un robot a través de una interfaz web interactiva. El **Client** controla el movimiento del robot y proporciona retroalimentación visual a través de LEDs. A continuación, se explican las funciones, configuración y características de cada componente.

## Server

El código del **Server** utiliza diversas librerías para controlar la cámara, manejar la conexión WiFi y establecer un servidor HTTP que permite visualizar el video en streaming y controlar el robot desde una página web.

### Librerías Utilizadas

- `esp_camera.h`: Control de la cámara en el ESP32.
- `WiFi.h`: Conexión a redes WiFi.
- `esp_timer.h`: Control de temporizadores y eventos.
- `img_converters.h`: Conversión de formatos de imagen.
- `fb_gfx.h`: Gráficos y funciones de dibujo.
- `soc/soc.h` y `soc/rtc_cntl_reg.h`: Configuración de registros y control de energía del ESP32.
- `esp_http_server.h`: Implementación de un servidor HTTP para manejar peticiones web.

### Configuración del Server

- **SSID y Contraseña**: Variables para la red WiFi a la que se conectará el ESP32.
- **Delimitador**: Parámetro para la transmisión de imágenes.
- **Modelo de Cámara**: Configuración del modelo de cámara (AI-THINKER).
- **Pines de la Cámara**: Definición de los pines de conexión de la cámara.

### Funciones Principales

- **index_handler**: Genera la página web principal, mostrando el streaming de video y los controles del robot.
- **stream_handler**: Controla la transmisión del video desde la cámara en formato JPEG, capturando y enviando frames al cliente.
- **cmd_handler**: Procesa los comandos recibidos del cliente para controlar el movimiento del robot y el estado del flash.

### Interfaz Web

La interfaz HTML proporcionada por el servidor permite a los usuarios controlar el robot a través de botones que dirigen sus movimientos y un interruptor para activar el flash. También incluye un deslizador (slider) para ajustar el ángulo de un servomotor que sostiene la cámara del robot. La interfaz se ha desarrollado con HTML, CSS y JavaScript para una experiencia de usuario fluida e interactiva.

---

## Client

El código del **Client** se ejecuta en un Arduino Pro Mini, y está diseñado para recibir comandos enviados desde el Server a través de la comunicación serie, controlando así el movimiento del robot y ajustando el ángulo de la cámara mediante un servomotor. También utiliza LEDs NeoPixel para indicar visualmente el estado del sistema y un sensor de efecto Hall para detectar la presencia de campos magnéticos cercanos.

### Librerías Utilizadas

- `Adafruit_NeoPixel.h`: Control de LEDs NeoPixel, usados para proporcionar retroalimentación visual.
- `Servo.h`: Control del servomotor para ajustar el ángulo de la cámara.

### Definiciones de Pines

- **Pines de Motor**: Se definen cuatro pines (`OUTA1`, `OUTA2`, `OUTB1`, `OUTB2`) para controlar los motores que mueven el robot.
- **Pin de LEDs**: Pin (`PIXELS_PIN`) para los LEDs NeoPixel y número total de LEDs (`NUMPIXELS`).
- **Pin de Servo**: Pin (`SERVO_PIN`) para controlar el servomotor.
- **Pin de Sensor de Efecto Hall**: Pin (`SENSOR_PIN`) para leer valores del sensor magnético, que ajusta el comportamiento visual de los LEDs.

### Inicialización del Sistema

- **Comunicación Serie**: Configuración de la comunicación serie a 9600 baudios.
- **Inicialización de LEDs**: Los LEDs se inicializan encendiéndose en rojo secuencialmente, indicando el inicio del sistema. Luego cambian a verde, mostrando que el sistema está listo.
- **Inicialización del Servo**: El servomotor se establece en un ángulo inicial de 90 grados.
- **Calibración del Sensor de Efecto Hall**: El sensor se calibra para obtener una lectura de referencia, permitiendo detectar cambios en campos magnéticos.

### Bucle Principal

- **Lectura de Comandos**: El programa monitorea continuamente el puerto serie en busca de comandos, que son procesados y ejecutados por la función `handleCommand()`.
- **Lectura del Sensor**: El sensor de efecto Hall se lee constantemente para calcular diferencias con la calibración inicial. Según la variación detectada, se ajusta el color de los LEDs para reflejar el cambio.

### Manejo de Comandos

La función `handleCommand(String cmd)` procesa los comandos recibidos para controlar el robot y el servomotor:

- **Comandos Disponibles**:
  - `forward`: Mueve el robot hacia adelante.
  - `right`: Gira el robot a la derecha.
  - `left`: Gira el robot a la izquierda.
  - `backward`: Mueve el robot hacia atrás.
  - `stop`: Detiene el movimiento.
  - `servo-<angle>`: Ajusta el servomotor al ángulo especificado (0° a 180°).

### Control de LEDs

La función `changeColor()` ajusta el color de los LEDs NeoPixel según la lectura del sensor de efecto Hall:

- **Verde**: Baja variación en la lectura del sensor.
- **Amarillo**: Variación moderada.
- **Rojo**: Alta variación, indicando presencia de un campo magnético fuerte.

### Funciones de Movimiento

Cada comando de movimiento ejecuta una función que configura los pines de los motores para lograr la dirección deseada:

- **moveForward()**: Avanza.
- **moveRight()**: Gira a la derecha.
- **moveLeft()**: Gira a la izquierda.
- **moveBackward()**: Retrocede.
- **stopMovement()**: Detiene el movimiento.

### Resumen

El **Client** recibe comandos a través de la comunicación serie y ejecuta acciones de movimiento en el robot, mientras los LEDs proporcionan retroalimentación visual y el servomotor ajusta el ángulo de la cámara, permitiendo una interfaz de control intuitiva y funcional.

---

## Resumen del Proyecto

Este proyecto combina hardware y software para crear una aplicación de control remoto con visualización en tiempo real. Su versatilidad permite aplicaciones en robótica, vigilancia, exploración y aprendizaje en programación de sistemas embebidos y motores.

## Requisitos

- **Hardware**: ESP32 con cámara AI-THINKER, Arduino Pro Mini, puente H, dos motores DC, un módulo step-down, tira de LEDs NeoPixel, servomotor, sensor de efecto Hall, y dos baterías 18650.
- **Software**: Arduino IDE o PlatformIO para programar el ESP32 y el Arduino Pro Mini.

## Instalación

1. Conectar el ESP32 y el Arduino Pro Mini a la computadora.
2. Cargar el código del **Server** en el ESP32 y el del **Client** en el Arduino Pro Mini mediante el entorno de desarrollo.
3. Asegurarse de que el ESP32 esté conectado a una red WiFi.
4. En un navegador web, ingresar la dirección IP del ESP32 para acceder a la interfaz web y controlar el robot.

## Contribuciones

Contribuciones y sugerencias son bienvenidas. Puedes enviar un pull request o abrir un issue para discutir nuevas ideas o características.

## Licencia

Este proyecto está licenciado bajo la Licencia MIT. Consulta el archivo LICENSE para obtener más detalles.
