#include "esp_camera.h"       // Librería para controlar la cámara con el ESP32.
#include <WiFi.h>             // Librería para la conexión WiFi.
#include "esp_timer.h"        // Librería para controlar tiempos y eventos.
#include "img_converters.h"   // Librería para conversión de imágenes.
#include "fb_gfx.h"           // Librería para gráficos.
#include "soc/soc.h"          // Librería para configurar el sistema-on-chip.
#include "soc/rtc_cntl_reg.h" // Librería para controlar registros RTC.
#include "esp_http_server.h"  // Librería para implementar un servidor HTTP.

const char* ssid = "Totalplay-C2AB"; // Nombre de la red WiFi.
const char* password = "C2AB50F6pdNhEJ8a"; // Contraseña de la red WiFi.

#define PART_BOUNDARY "123456789000000000000987654321" // Delimitador de partes para transmisión de imágenes.

#define CAMERA_MODEL_AI_THINKER // Define el modelo de la cámara.

  // Pines de la cámara AI-THINKER.
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22

#define FLASH_LED_PIN 4 // Pin para controlar el LED del flash.

// Configuración de tipo de contenido para el streaming de imágenes.
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// Manejadores de servidor HTTP.
httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

// Página HTML para controlar el robot y mostrar el stream.
static const char PROGMEM INDEX_HTML[] = R"rawliteral(

  <!DOCTYPE html>
  <html lang="es-MX">
  
    <head>
  
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  
      <style>
  
        body {
          background-color: #070714;
          font-family: Arial, Helvetica, sans-serif;
          text-align: center;
          margin: 0 auto;
          padding: 0;
        }
  
        h1 {
          color: #CCC;
          font-weight: 800;
          -webkit-touch-callout: none;
          -webkit-user-select: none;
          -khtml-user-select: none;
          -moz-user-select: none;
          -ms-user-select: none;
          user-select: none;
          -webkit-tap-highlight-color: transparent;
        }
  
        .container {
          display: flex;
          justify-content: center;
          align-items: center;
        }
  
        .image-container {
          flex: 1;
          max-width: 85%;
        }
  
        table {
          margin-left: auto;
          margin-right: auto;
          flex: 1;
        }
  
        .button {
          background-color: #0E0E2C;
          border: 3px solid #838282;
          border-radius: 15px;
          padding: 12px 12px;
          width: 65px;
          height: 65px;
          display: flex;
          justify-content: center;
          align-items: center;
          cursor: pointer;
          transition: transform 0.3s ease, box-shadow 0.3s ease;
          outline: none;
          -webkit-touch-callout: none;
          -webkit-user-select: none;
          -khtml-user-select: none;
          -moz-user-select: none;
          -ms-user-select: none;
          user-select: none;
          -webkit-tap-highlight-color: transparent;
        }
  
        .button:active {
          background-color: #1D1D44;
        }
  
        .button:hover {
          transform: scale(1.05);
          box-shadow: 0 8px 15px #A57AF54D;
        }
  
        .center {
          background-color: #101014;
          border-radius: 50%;
          width: 75px;
          height: 75px;
        }
  
        img {
          width: auto;
          max-width: 100%;
          height: auto;
        }
  
        .arrow {
          border: solid #7C3FEC;
          border-width: 0 10px 10px 0;
          display: inline-block;
          padding: 5px;
        }
  
        .right {
          transform: rotate(-45deg);
          -webkit-transform: rotate(-45deg);
        }
  
        .left {
          transform: rotate(135deg);
          -webkit-transform: rotate(135deg);
        }
  
        .up {
          transform: rotate(-135deg);
          -webkit-transform: rotate(-135deg);
        }
  
        .down {
          transform: rotate(45deg);
          -webkit-transform: rotate(45deg);
        }
  
        .light {
          font-size:xx-large;
          padding: 5px;
        }
  
        input[type="range"] {
          margin-top: 10px;
          -webkit-appearance: none;
          appearance: none;
          border-radius: 20px;
        }
  
        input[type="range"]::-webkit-slider-thumb {
          -webkit-appearance: none;
          appearance: none;
          width: 20px;
          height: 20px;
          background: #4409B3;
          border-radius: 50%;
          cursor: pointer;
          margin-top: -2px;
        }
  
        input[type="range"]::-moz-range-thumb {
          width: 20px;
          height: 20px;
          background: #4409B3;
          border-radius: 50%;
          cursor: pointer;
          margin-top: -2px;
        }
  
        input[type="range"]::-ms-thumb {
          width: 20px;
          height: 20px;
          background: #4409B3;
          border-radius: 50%;
          cursor: pointer;
          margin-top: -2px;
        }
  
        input[type="range"]::-webkit-slider-runnable-track {
          height: 16px;
          background: #B48CFF;
          border-radius: 20px;
        }
  
        input[type="range"]::-moz-range-track {
          height: 15px;
          background: #B48CFF;
          border-radius: 20px;
        }
  
        input[type="range"]::-ms-track {
          height: 16px;
          background: #B48CFF;
          border-radius: 20px;
          border: none;
          color: transparent;
        }
  
        #angleValue {
          color: #CCC;
          font-size: x-large;
          font-weight: 800;
        }
  
        @media(min-width: 900px) {
          .main-container {
            display: flex;
            flex-direction: row;
            align-items: flex-start;
          }
          td {
            padding: 24px;
          }
          input[type="range"] {
            width: 55%;
          }
        }
  
        @media(max-width: 900px) {
          .main-container {
            display: flex;
            flex-direction: column;
            align-items: center;
          }
          td {
            padding: 8px;
          }
          input[type="range"] {
            width: 85%;
          }
        }
  
      @media (hover: none) {
        .button:hover {
          transform: none;
          box-shadow: none;
        }
        .button:active {
          background-color: #1D1D44;
          transform: scale(1.05);
          box-shadow: 0 8px 15px #A57AF54D;
        }
  
      }
  
      </style>
  
      <title>Dashboard 🌕</title>
  
    </head>
  
    <body>
  
      <h1>COLMENA 🤖</h1>
  
      <div class="main-container">
        
        <div class="container image-container">
          <img src="" id="photo">
        </div>
  
        <table>
          <tr>
            <td colspan="3" align="center">
              <button class="button" onmousedown="sendCommand('forward');" ontouchstart="sendCommand('forward');"
                onmouseup="sendCommand('stop');" ontouchend="sendCommand('stop');">
                <i class="arrow up"></i>
              </button>
            </td>
          </tr>
          <tr>
            <td align="center">
              <button class="button" onmousedown="sendCommand('left');" ontouchstart="sendCommand('left');"
                onmouseup="sendCommand('stop');" ontouchend="sendCommand('stop');">
                <i class="arrow left"></i>
              </button>
            </td>
            <td align="center">
              <button class="button center" id="flashButton" onclick="toggleFlash();"><span class="light">💡</span></button>
            </td>
            <td align="center">
              <button class="button" onmousedown="sendCommand('right');" ontouchstart="sendCommand('right');"
                onmouseup="sendCommand('stop');" ontouchend="sendCommand('stop');">
                <i class="arrow right"></i>
              </button>
            </td>
          </tr>
          <tr>
            <td colspan="3" align="center">
              <button class="button" onmousedown="sendCommand('backward');" ontouchstart="sendCommand('backward');"
                onmouseup="sendCommand('stop');" ontouchend="sendCommand('stop');">
                <i class="arrow down"></i>
              </button>
            </td>
          </tr>
          <tr>
            <td colspan="3" align="center">
                <input type="range" id="angleSlider" min="-45" max="45" value="0" step="5" oninput="updateAngle(this.value)" />
                <p id="angleValue">Servo: 0°</p>
            </td>
        </tr>
        </table>
  
      </div>
  
      <script>
  
        let flashOn = false;
  
        function sendCommand(action) {
          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/action?go=" + action, true);
          xhr.send();
        }
  
        function toggleFlash() {
  
          flashOn = !flashOn;
  
          const button = document.getElementById("flashButton");
  
          if (flashOn) {
            sendCommand("flash-on");
            button.style.backgroundColor = "#F4F4F4";
          } else {
            sendCommand("flash-off");
            button.style.backgroundColor = "#101014"; 
          }
  
        }
  
        function updateAngle(value) {
          document.getElementById('angleValue').innerText = "Servo: " + value + "°";
          sendCommand("servo-" + value);
        }
  
        window.onload = function() {
  
          const button = document.getElementById("flashButton");
          sendCommand("flash-off");
  
          const slider = document.getElementById('angleValue');
          sendCommand("servo-0");
  
          document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
  
        };
  
      </script>
  
    </body>
  
  </html>
  
)rawliteral";

// Manejador de la página principal.
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

// Manejador de la transmisión de la cámara.
static esp_err_t stream_handler(httpd_req_t *req) {
  
  camera_fb_t * fb = NULL; // Frame buffer para la imagen.
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0; // Tamaño del buffer JPEG.
  uint8_t * _jpg_buf = NULL; // Buffer de la imagen JPEG.
  char * part_buf[64]; // Buffer para las partes de la imagen.

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE); // Establece el tipo de contenido.
  
  if(res != ESP_OK){
    return res;
  }

  while(true){
    
    fb = esp_camera_fb_get(); // Captura un frame de la cámara.
    
    if (!fb) {
      res = ESP_FAIL; // Falla si no se puede obtener el frame.
    } else {
      
      if(fb->width > 400){ // Procesa si el ancho del frame es mayor a 400.
        
        if(fb->format != PIXFORMAT_JPEG){
          
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len); // Convierte a JPEG.
          esp_camera_fb_return(fb); // Devuelve el frame.
          fb = NULL;
          
          if(!jpeg_converted){
            res = ESP_FAIL; // Falla si no se puede convertir.
          }
          
        } else {
          _jpg_buf_len = fb->len; // Longitud del buffer.
          _jpg_buf = fb->buf; // Buffer de la imagen.
        }
        
      }
      
    }
    
    // Envía las partes de la imagen al cliente.
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb); // Devuelve el frame.
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf); // Libera el buffer.
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break; // Sale del loop si ocurre un error.
    }
    
  }
  
  return res;
  
}

// Manejador para los comandos del robot.
static esp_err_t cmd_handler(httpd_req_t *req) {
  
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
  
  buf_len = httpd_req_get_url_query_len(req) + 1;
  
  if (buf_len > 1) {
    
    buf = (char*)malloc(buf_len);
    
    if(!buf){
      httpd_resp_send_500(req); // Error de servidor.
      return ESP_FAIL;
    }
    
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      
      if (httpd_query_key_value(buf, "go", variable, sizeof(variable)) == ESP_OK) {
        // Aquí se verifica si se ha recibido una acción válida.
      } else {
        free(buf);
        httpd_resp_send_404(req); // No se encontró la variable.
        return ESP_FAIL;
      }
      
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    
    free(buf);
    
  } else {
    httpd_resp_send_404(req); // Sin parámetros en la consulta.
    return ESP_FAIL;
  }

  // Control del estado del robot y el flash.
  if(!strcmp(variable, "forward")) {
    Serial.println("forward"); // Mover hacia adelante.
  }
  else if(!strcmp(variable, "right")) {
    Serial.println("right"); // Girar a la derecha.
  }
  else if(!strcmp(variable, "left")) {
    Serial.println("left"); // Girar a la izquierda.
  }
  else if(!strcmp(variable, "backward")) {
    Serial.println("backward"); // Mover hacia atrás.
  }
  else if(!strcmp(variable, "stop")) {
    Serial.println("stop"); // Detener movimiento.
  }
  else if (!strcmp(variable, "flash-on")) {
    digitalWrite(FLASH_LED_PIN, HIGH); // Encender LED.
  }
  else if (!strcmp(variable, "flash-off")) {
    digitalWrite(FLASH_LED_PIN, LOW); // Apagar LED.
  }
  else if (String(variable).startsWith("servo-")) {
    // Controlar servo basado en el comando recibido.
    int angle = map(String(variable).substring(6).toInt(), -45, 45, 180, 0);
    Serial.println("servo-" + String(angle)); // Mover servo a un ángulo específico.
  } else {
    return httpd_resp_send_500(req); // Respuesta de error para comandos no válidos.
  }

  // Permitir solicitudes de otros dominios
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0); // Respuesta exitosa.
}

// Inicializa y configura el servidor de la cámara.
void startCameraServer() {
  
  // Configuración inicial del servidor HTTP usando configuraciones predeterminadas.
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80; // Establece el puerto del servidor en 80.
  
  // Configuración para la URI del índice (página principal).
  httpd_uri_t index_uri = {
    .uri       = "/", // URI principal
    .method    = HTTP_GET, // Método HTTP permitido
    .handler   = index_handler, // Función manejadora para esta URI
    .user_ctx  = NULL // Contexto de usuario opcional (no utilizado aquí)
  };
  
  // Configuración para la URI de comandos (acción del robot).
  httpd_uri_t cmd_uri = {
    .uri       = "/action", // URI para recibir comandos
    .method    = HTTP_GET, // Método HTTP permitido
    .handler   = cmd_handler, // Función manejadora para esta URI
    .user_ctx  = NULL // Contexto de usuario opcional (no utilizado aquí)
  };
  
  // Configuración para la URI de transmisión de video.
  httpd_uri_t stream_uri = {
    .uri       = "/stream", // URI para la transmisión
    .method    = HTTP_GET, // Método HTTP permitido
    .handler   = stream_handler, // Función manejadora para esta URI
    .user_ctx  = NULL // Contexto de usuario opcional (no utilizado aquí)
  };
  
  // Inicia el servidor HTTP para manejar las solicitudes de la cámara.
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri); // Registra el manejador para la URI de índice.
    httpd_register_uri_handler(camera_httpd, &cmd_uri); // Registra el manejador para la URI de comandos.
  }
  
  // Configura un nuevo puerto para el servidor de transmisión.
  config.server_port += 1; // Cambia el puerto del servidor para la transmisión.
  config.ctrl_port += 1; // Cambia el puerto de control también.
  
  // Inicia el servidor HTTP para la transmisión de video.
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri); // Registra el manejador para la URI de transmisión.
  }
  
}

// Configura la cámara y la conexión WiFi al iniciar.
void setup() {
  
  // Desactiva el sistema de protección de sobrevoltaje (brown-out) en el RTC.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  // Configura el pin del LED de flash como salida.
  pinMode(FLASH_LED_PIN, OUTPUT);

  // Inicializa la comunicación serie a 9600 baudios.
  Serial.begin(9600);
  Serial.setDebugOutput(false); // Desactiva la salida de depuración por serie.

  // Estructura de configuración para la cámara.
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0; // Canal LEDC utilizado para la señal de la cámara.
  config.ledc_timer = LEDC_TIMER_0; // Temporizador utilizado para la señal de la cámara.
  config.pin_d0 = Y2_GPIO_NUM; // Pin GPIO para el dato 0.
  config.pin_d1 = Y3_GPIO_NUM; // Pin GPIO para el dato 1.
  config.pin_d2 = Y4_GPIO_NUM; // Pin GPIO para el dato 2.
  config.pin_d3 = Y5_GPIO_NUM; // Pin GPIO para el dato 3.
  config.pin_d4 = Y6_GPIO_NUM; .
  config.pin_d5 = Y7_GPIO_NUM; // Pin GPIO para el dato 5.
  config.pin_d6 = Y8_GPIO_NUM; // Pin GPIO para el dato 6.
  config.pin_d7 = Y9_GPIO_NUM; // Pin GPIO para el dato 7.
  config.pin_xclk = XCLK_GPIO_NUM; // Pin para la señal de reloj.
  config.pin_pclk = PCLK_GPIO_NUM; // Pin para el reloj del pixel.
  config.pin_vsync = VSYNC_GPIO_NUM; // Pin para la señal de sincronización vertical.
  config.pin_href = HREF_GPIO_NUM; // Pin para la señal de referencia de línea.
  config.pin_sscb_sda = SIOD_GPIO_NUM; // Pin para la línea de datos del bus I2C.
  config.pin_sscb_scl = SIOC_GPIO_NUM; // Pin para la línea de reloj del bus I2C.
  config.pin_pwdn = PWDN_GPIO_NUM; // Pin para controlar el modo de alimentación de la cámara.
  config.pin_reset = RESET_GPIO_NUM; // Pin para el reset de la cámara.
  config.xclk_freq_hz = 20000000; // Frecuencia del reloj de entrada para la cámara.
  config.pixel_format = PIXFORMAT_JPEG; // Formato de pixel, en este caso JPEG.
  config.frame_size = FRAMESIZE_VGA; // Tamaño del cuadro de la imagen (640x480).
  config.jpeg_quality = 10; // Calidad JPEG, de 0 a 63 (menor es mejor calidad).
  config.fb_count = 2; // Número de buffers de marco.

  // Inicializa la cámara con la configuración especificada.
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) { // Verifica si hubo un error al inicializar la cámara.
    return; // Sale de la función si la inicialización falla.
  }

  // Obtiene el sensor de la cámara y configura la inversión vertical y horizontal.
  sensor_t *sensor = esp_camera_sensor_get();
  sensor->set_vflip(sensor, 1); // Activa la inversión vertical de la imagen.
  sensor->set_hmirror(sensor, 1); // Activa el espejo horizontal de la imagen.

  // Conecta la ESP32 a la red WiFi usando el SSID y contraseña proporcionados.
  WiFi.begin(ssid, password);
  // Espera hasta que la conexión WiFi esté establecida.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Espera medio segundo antes de verificar nuevamente.
  }
  
  // Inicia el servidor de la cámara para manejar las solicitudes HTTP.
  startCameraServer();
  
}

// Bucle principal que no realiza ninguna acción en este caso.
void loop() {
  
}
