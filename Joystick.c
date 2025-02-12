#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

// Definindo os pinos do LED RGB
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

// Definindo os pinos dos botões
#define BUTTON_A 5
#define BUTTON_JOYSTICK 22

// Variáveis para leitura da diração do Joystick
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y

// Debounce time
#define DEBOUNCE_TIME 200  // 200 ms

// Estado dos LEDs e botões
volatile bool state_green_led = false;
volatile bool state_blue_led = false;
volatile bool state_red_led = false;
volatile uint32_t last_press_time_A = 0;
volatile uint32_t last_press_time_J = 0;

// Variáveis para a comunicação I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define SSD1306_ADDR 0x3C
ssd1306_t ssd;

// Variáveis para controle de PWM
#define PWM_DIVISER 150.0 // Divisor de clock para PWM
#define WRAP_VALUE 4096 // WRAP para o Joystick
volatile bool pwm_enabled = false; // Estado do PWM (se ativado ou não)

// Estado da borda do display
volatile bool state_border = false;
volatile bool state_shape = false;  // Controla se o quadrado deve aparecer

// Posição do quadrado no display (inicialmente centralizado)
volatile int square_x = 60;
volatile int square_y = 28;
#define SQUARE_SIZE 8  // Tamanho do quadrado
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Função para configurar os pinos GPIO
void config_gpio() {
    // Inicializando e configurando LEDs como saída
    gpio_init(LED_GREEN);
    gpio_init(LED_BLUE);
    gpio_init(LED_RED);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);

    // Inicializando os botões como entrada e resistores pull-up
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_JOYSTICK);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_JOYSTICK, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_JOYSTICK);

    // Inicializa as funções ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN); 
}

// Função para configurar o módulo PWM
void pwm_setup() {
    // Configura o PWM para o LED Azul
    gpio_set_function(LED_BLUE, GPIO_FUNC_PWM);
    uint slice_led_blue = pwm_gpio_to_slice_num(LED_BLUE);
    pwm_set_clkdiv(slice_led_blue, PWM_DIVISER);
    pwm_set_wrap(slice_led_blue, WRAP_VALUE);
    pwm_set_enabled(slice_led_blue, true);

    // Configura o PWM para o LED Vermelho
    gpio_set_function(LED_RED, GPIO_FUNC_PWM);
    uint slice_led_red = pwm_gpio_to_slice_num(LED_RED);
    pwm_set_clkdiv(slice_led_red, PWM_DIVISER);
    pwm_set_wrap(slice_led_red, WRAP_VALUE);
    pwm_set_enabled(slice_led_red, true);
}

// Função para definir brilho dos LEDs com base no Joystick (somente se o PWM estiver ativado)
void set_led_intensity(uint16_t joystick_x, uint16_t joystick_y) {
    if (pwm_enabled) {
        // Calcula a intensidade dos LEDs baseada na posição do joystick
        uint16_t intensity_x = abs(joystick_x - 2048) * 2;  // X controla LED Vermelho
        uint16_t intensity_y = abs(joystick_y - 2048) * 2;  // Y controla LED Azul

        // Define o nível de PWM para os LEDs
        pwm_set_gpio_level(LED_BLUE, intensity_y);
        pwm_set_gpio_level(LED_RED, intensity_x);
    } else {
        // Se o PWM estiver desativado, mantém os LEDs apagados
        pwm_set_gpio_level(LED_BLUE, 0);
        pwm_set_gpio_level(LED_RED, 0);
    }
}

// Função para desenhar borda tracejada
void draw_dashed_border(ssd1306_t *display) {
    for (int i = 0; i < 128; i += 4) {
        ssd1306_pixel(display, i, 0, true);     // Linha superior
        ssd1306_pixel(display, i, 61, true);    // Linha inferior
    }
    for (int i = 0; i < 64; i += 4) {
        ssd1306_pixel(display, 0, i, true);     // Linha esquerda
        ssd1306_pixel(display, 127, i, true);   // Linha direita
    }
}

// Atualiza a tela para manter a borda e o desenho do botão A
void update_display() {
    ssd1306_fill(&ssd, false); // Limpa o display

    // Se a borda estiver ativada, desenha a borda
    if (state_border) {
        ssd1306_rect(&ssd, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, true, false);
    } else {
        draw_dashed_border(&ssd);
    }

    // Se o botão A ativou o desenho, desenha o retângulo
    if (state_shape) {
        ssd1306_rect(&ssd, square_y, square_x, SQUARE_SIZE, SQUARE_SIZE, true, true);
    }

    // Atualiza o display
    ssd1306_send_data(&ssd);
}

// Callback para interrupções dos botões
void button_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (gpio == BUTTON_A) { // Aciona os LEDs Vermelho e azul por PWM
        if (current_time - last_press_time_A > DEBOUNCE_TIME * 1000) { // Debounce
            last_press_time_A = current_time;  // Atualiza o tempo da última pressão

            pwm_enabled = !pwm_enabled; // Alterna o estado do PWM
            printf("Botão A pressionado. PWM Ativo: %d\n", pwm_enabled);
            state_shape = !state_shape;  // Alterna o estado do quadrado
            update_display();  // Atualiza o display
        }
    } 
    else if (gpio == BUTTON_JOYSTICK) { // Liga/Desliga o LED Verde e controla a borda do display
        if (current_time - last_press_time_J > DEBOUNCE_TIME * 1000) { // Debounce
            last_press_time_J = current_time;  // Atualiza o tempo da última pressão

            state_green_led = !state_green_led;  // Alterna o estado
            gpio_put(LED_GREEN, state_green_led);
            printf("Botão Joystick pressionado. LED Verde: %d\n", state_green_led);

            // Alterna a borda
            state_border = !state_border;
            update_display();
        }
    }
}

int main() {
    stdio_init_all();
    config_gpio();
    pwm_setup();

    // Inicialização do I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line

    // Inicializa o display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display inicialmente
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configura as interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);

    uint16_t joystick_x_position, joystick_y_position; 

    while (true) {
        adc_select_input(1); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
        joystick_x_position = adc_read();
        adc_select_input(0); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
        joystick_y_position = adc_read(); 

        set_led_intensity(joystick_x_position, joystick_y_position);

        if (state_shape) {
            square_x = (joystick_x_position * (SCREEN_WIDTH - SQUARE_SIZE)) / 4095;
            square_y = ((4095 - joystick_y_position) * (SCREEN_HEIGHT - SQUARE_SIZE)) / 4095;
            update_display();
        }
        printf("Joystick X: %d, Joystick Y: %d\n", joystick_x_position, joystick_y_position);

        float porcentagemX = (joystick_x_position/4096.0)*100;
        float porcentagemY = (joystick_y_position/4096.0)*100;

        printf("Nível do LED Verm. : %.2f, Nível do LED Azul: %.2f\n", porcentagemX, porcentagemY);

        sleep_ms(50);
    }
}