#include <stdio.h>
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

// Estado da borda do display
volatile bool state_border = false;
volatile bool state_shape = false;  // Controla se o quadrado deve aparecer

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
}

// Função para desenhar borda tracejada
void draw_dashed_border(ssd1306_t *display) {
    for (int i = 0; i < 128; i += 5) {
        ssd1306_pixel(display, i, 0, true);     // Linha superior
        ssd1306_pixel(display, i, 63, true);    // Linha inferior
    }
    for (int i = 0; i < 64; i += 5) {
        ssd1306_pixel(display, 0, i, true);     // Linha esquerda
        ssd1306_pixel(display, 127, i, true);   // Linha direita
    }
}

// Atualiza a tela para manter a borda e o desenho do botão A
void update_display() {
    ssd1306_fill(&ssd, false); // Limpa o display

    // Se a borda estiver ativada, desenha a borda
    if (state_border) {
        ssd1306_rect(&ssd, 0, 0, 127, 63, true, false);
    } else {
        draw_dashed_border(&ssd);
    }

    // Se o botão A ativou o desenho, desenha o retângulo
    if (state_shape) {
        ssd1306_rect(&ssd, 28, 60, 8, 8, true, true);
    }

    // Atualiza o display
    ssd1306_send_data(&ssd);
}

// Callback para interrupções dos botões
void button_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (gpio == BUTTON_A) { // Aciona os LEDs Vermelho e azul por PWM
        if (current_time - last_press_time_A > DEBOUNCE_TIME * 1000) { // Debounce
            printf("Botão A pressionado: PWM ativado");
            last_press_time_A = current_time;  // Atualiza o tempo da última pressão
            // Lógica para a alteração por PWM

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
            update_display();  // Atualiza o display mantendo tudo visível
        }
    }
}

int main() {
    stdio_init_all();
    config_gpio();

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

    while (true) {
    }
}