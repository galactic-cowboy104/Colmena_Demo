#include "esp_camera.h"       // Librería para controlar la cámara con el ESP32.
#include <WiFi.h>             // Librería para la conexión WiFi.
#include "esp_timer.h"        // Librería para controlar tiempos y eventos.
#include "img_converters.h"   // Librería para conversión de imágenes.
#include "fb_gfx.h"           // Librería para gráficos.
#include "soc/soc.h"          // Librería para configurar el sistema-on-chip.
#include "soc/rtc_cntl_reg.h" // Librería para controlar registros RTC.
#include "esp_http_server.h"  // Librería para implementar un servidor HTTP.

const char* ssid = ""; // Nombre de la red WiFi.
const char* password = ""; // Contraseña de la red WiFi.

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
bool flashState = LOW; // Estado inicial del LED del flash (apagado).

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
      <meta name="viewport" content="width=device-width, initial-scale=1">
  
      <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.6.0/css/all.min.css" 
            integrity="sha512-Kc323vGBEqzTmouAECnVceyQqyqdsSiqLQISBL29aUW4U/M7pSPA/gEUZQqv1cwx4OnYxTxve5UMg5GT6L4JJg==" 
            crossorigin="anonymous" referrerpolicy="no-referrer"/>
  
      <style>
  
        body {
          font-family: Arial, Helvetica, sans-serif;
          background-color: #071214;
          text-align: center;
          margin: 0 auto;
          padding: 0;
        }
  
        h1 {
          color: #CCC;
          font-weight: 900;
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
          max-width: 100%;
        }
  
        table {
          margin-left: auto;
          margin-right: auto;
          flex: 1;
        }
  
        td {
          padding: 8px;
        }
  
        .button {
          background-color: #113233;
          border: 3px solid #838282;
          border-radius: 15px;
          padding: 12px 12px;
          width: 75px;
          cursor: pointer;
          -webkit-touch-callout: none;
          -webkit-user-select: none;
          -khtml-user-select: none;
          -moz-user-select: none;
          -ms-user-select: none;
          user-select: none;
          -webkit-tap-highlight-color: transparent;
        }
  
        .button:active {
          background-color: #2f3636;
        }
  
        img {
          width: auto;
          max-width: 100%;
          height: auto;
        }
  
        .switch {
          position: relative;
          display: inline-block;
          width: 60px;
          height: 34px;
          -webkit-touch-callout: none;
          -webkit-user-select: none;
          -khtml-user-select: none;
          -moz-user-select: none;
          -ms-user-select: none;
          user-select: none;
          -webkit-tap-highlight-color: transparent;
        }
  
        .switch input { 
          opacity: 0;
          width: 0;
          height: 0;
        }
  
        .slider {
          position: absolute;
          cursor: pointer;
          top: 0;
          left: 0;
          right: 0;
          bottom: 0;
          background-color: #ccc;
          -webkit-transition: .4s;
          transition: .4s;
        }
  
        .slider:before {
          position: absolute;
          content: "";
          height: 26px;
          width: 26px;
          left: 4px;
          bottom: 4px;
          background-color: white;
          -webkit-transition: .4s;
          transition: .4s;
        }
  
        input:checked + .slider {
          background-color: #2196F3;
        }
  
        input:focus + .slider {
          box-shadow: 0 0 1px #2196F3;
        }
  
        input:checked + .slider:before {
          -webkit-transform: translateX(26px);
          -ms-transform: translateX(26px);
          transform: translateX(26px);
        }
  
        .slider.round {
          border-radius: 34px;
        }
  
        .slider.round:before {
          border-radius: 50%;
        }
  
        .flash-label {
          color: #E9DADA;
          font-size: x-large;
          font-weight: 700;
          -webkit-touch-callout: none;
          -webkit-user-select: none;
          -khtml-user-select: none;
          -moz-user-select: none;
          -ms-user-select: none;
          user-select: none;
          -webkit-tap-highlight-color: transparent;
        }
  
        @media(min-width: 900px) {
          .main-container {
            display: flex;
            flex-direction: row;
            align-items: flex-start;
          }
        }
  
        @media(max-width: 900px) {
          .main-container {
            display: flex;
            flex-direction: column;
            align-items: center;
          }
        }
  
      </style>
  
      <title>Colmena Robot 🤖</title>
  
    </head>
  
    <body>
  
      <h1>Colmena Robot 🤖</h1>
  
      <div class="main-container">
        
        <div class="container image-container">
          <img src="" id="photo">
        </div>
  
        <table>
          <tr>
            <td colspan="3" align="center">
              <button class="button" onmousedown="toggleCheckbox('forward');" ontouchstart="toggleCheckbox('forward');"
                onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">
                <i class="fa-solid fa-arrow-up fa-3x" style="color: #009b00;"></i>
              </button>
            </td>
          </tr>
          <tr>
            <td align="center">
              <button class="button" onmousedown="toggleCheckbox('left');" ontouchstart="toggleCheckbox('left');"
                onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">
                <i class="fa-solid fa-arrow-left fa-3x" style="color: #009b00;"></i>
              </button>
            </td>
            <td align="center">
              <label class="switch">
                <input type="checkbox" id="flash-switch">
                <span class="slider round"></span>
              </label>
            </td>
            <td align="center">
              <button class="button" onmousedown="toggleCheckbox('right');" ontouchstart="toggleCheckbox('right');"
                onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">
                <i class="fa-solid fa-arrow-right fa-3x" style="color: #009b00;"></i>
              </button>
            </td>
          </tr>
          <tr>
            <td colspan="3" align="center">
              <button class="button" onmousedown="toggleCheckbox('backward');" ontouchstart="toggleCheckbox('backward');"
                onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">
                <i class="fa-solid fa-arrow-down fa-3x" style="color: #009b00;"></i>
              </button>
            </td>
          </tr>
        </table>
      </div>
  
      <script>
  
        function toggleCheckbox(action) {
          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/action?go=" + action, true);
          xhr.send();
        }
  
        window.onload = function() {
  
          document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
          
          document.getElementById("flash-switch").addEventListener("change", function() {
            if (this.checked) {
              toggleCheckbox('flash-on');
            } else {
              toggleCheckbox('flash-off');
            }
          });
  
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
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  // Control del estado del robot y el flash.
  if(!strcmp(variable, "forward")) {
    Serial.println("forward");
  }
  else if(!strcmp(variable, "right")) {
    Serial.println("right");
  }
  else if(!strcmp(variable, "left")) {
    Serial.println("left");
  }
  else if(!strcmp(variable, "backward")) {
    Serial.println("backward");
  }
  else if(!strcmp(variable, "stop")) {
    Serial.println("stop");
  }
  else if (!strcmp(variable, "flash-on")) {
    flashState = true;
    digitalWrite(FLASH_LED_PIN, flashState);
  }
  else if (!strcmp(variable, "flash-off")) {
    flashState = false;
    digitalWrite(FLASH_LED_PIN, flashState);
  } else {
    return httpd_resp_send_500(req);
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
  
}

// Inicializa y configura el servidor de la cámara.
void startCameraServer() {
  
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  
  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };
  
  httpd_uri_t cmd_uri = {
    .uri       = "/action",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
  };
  
  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };
  
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
  }
  
  config.server_port += 1;
  config.ctrl_port += 1;
  
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
  
}

// Configura la cámara y la conexión WiFi al iniciar.
void setup() {
  
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0)
  ;
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState);

  Serial.begin(9600);
  Serial.setDebugOutput(false);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  startCameraServer();

}

void loop() {
  delay(1);
}
