# 📌 Controle de LEDs e Display OLED com Joystick no RP2040

## 📖 Sobre o Projeto
Este projeto demonstra o **uso do PWM no Raspberry Pi Pico W** para controlar **LEDs RGB e um display OLED SSD1306** utilizando um **joystick analógico**. Além disso, os botões permitem alternar entre diferentes estados e estilos de borda no display.

## 🎯 Objetivos do Projeto
✅ Compreender o funcionamento do **Conversor Analógico-Digital (ADC) no RP2040**.  
✅ Utilizar **PWM** para controlar a intensidade de dois LEDs RGB com base nos valores do **joystick**.  
✅ Representar a posição do joystick no **display SSD1306** por meio de um **quadrado móvel**.  
✅ Aplicar o protocolo de comunicação **I2C** para integração com o display OLED.  
✅ Implementar o controle do LED Verde e alternância de bordas com botões.  

## 🛠️ Componentes Utilizados
- **Microcontrolador:** Raspberry Pi Pico W  
- **LED RGB (Verde, Azul e Vermelho)** – GPIOs **11, 12 e 13**  
- **Joystick analógico** – GPIOs **26 e 27**  
- **Botão A** – GPIO **5**  
- **Botão do Joystick** – GPIO **22**  
- **Display OLED SSD1306** – Conectado via **I2C (GPIOs 14 e 15)**  

## 🚀 Configuração e Funcionamento

### 1. Configuração do PWM para LEDs
- O PWM é usado para ajustar o brilho do **LED Azul** (baseado no eixo Y) e do **LED Vermelho** (baseado no eixo X).  
- A intensidade varia de acordo com a movimentação do **joystick**.  

### 2. Movimento do Quadrado no Display SSD1306
- O quadrado de **8x8 pixels** inicialmente centralizado **se move** conforme o joystick.  
- O eixo **X controla o deslocamento horizontal** e o eixo **Y controla o deslocamento vertical**.
- A movimentação é feita proporcionalmente ao valor lido dos ADCs.
- Há uma correção feita na movimentação do quadrado no eixo Y, já que, por padrão, as direções são invertidas.  

### 3. Funcionalidade dos Botões
- **Botão A (GPIO 5):**
  - **Ativa/desativa o PWM** para os LEDs.
  - Alterna a exibição do **quadrado móvel** no display.

- **Botão do Joystick (GPIO 22):**
  - Alterna o estado do **LED Verde**.
  - Alterna entre **diferentes estilos de borda** no display.

## 🛠️ Configuração do Hardware
1. **Conectar os componentes ao Raspberry Pi Pico W** conforme a seguinte tabela:

| Componente | GPIO |
|------------|------|
| LED Verde | 11 |
| LED Azul | 12 |
| LED Vermelho | 13 |
| Joystick X | 26 |
| Joystick Y | 27 |
| Botão A | 5 |
| Botão do Joystick | 22 |
| Display SSD1306 (I2C) | 14 (SDA), 15 (SCL) |

2. **Configurar PWM no código** para operar corretamente nos LEDs.  
3. **Ler os valores do joystick** via ADC e mapear para a movimentação do quadrado.  
4. **Implementar o tratamento de bordas no display**, alternando estilos conforme o botão do joystick.  

## 🔬 Resultados e Experimentos
Para validar o comportamento do sistema, foram realizados experimentos utilizando a **BitDogLab**, observando-se que:
1. **O LED Azul e Vermelho variam** sua intensidade de brilho conforme a posição do joystick.  
2. **O movimento do quadrado no display é fluído**, correspondendo à movimentação do joystick.  
3. **A borda do display alterna corretamente** entre os estilos ao pressionar o botão do joystick.  

## 📸 Demonstração do Projeto
📹 **[Assista ao vídeo demonstrativo](https://drive.google.com/file/d/1WAVL-4zYL3jSMOv-eAbYWNc72i3YzBPj)**  