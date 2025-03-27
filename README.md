# Analisador de Espectro de Áudio - Projeto BitDogLab - Virtus CC

![Logo do Projeto](/documentation/virtuscc-logo.png)

## Visão Geral
Este projeto implementa um analisador de espectro de áudio em tempo real utilizando um Raspberry Pi Pico W, com entrada de microfone e display OLED SSD1306. O dispositivo captura sinais de áudio, aplica técnicas de processamento digital de sinais e visualiza o espectro de frequência na faixa de voz humana.

## 🚀 Recursos Principais
- Análise de frequência de áudio em tempo real
- Filtragem passa-baixa de sinais de áudio
- Implementação de Transformada Rápida de Fourier (FFT)
- Visualização gráfica em display OLED
- Foco na faixa de frequência da voz humana (85 Hz - 255 Hz)

## 🖼️ Galeria do Microcontrolador

| Imagem 1 | Imagem 2 |
|----------|----------|
| ![Imagem 1](/documentation/image-bitdoglab-1.png) | ![Imagem 2](/documentation/image-bitdoglab-2.png) |

| Imagem 3 | Imagem 4 |
|----------|----------|
| ![Imagem 3](/documentation/image-bitdoglab-3.png) | ![Imagem 4](/documentation/image-bitdoglab-4.png) |

## 📋 Requisitos de Hardware
- Raspberry Pi Pico W
- Display OLED SSD1306
- Módulo de microfone
- Protoboard e cabos jumper

## 🔧 Configuração de Pinos
- Comunicação I2C:
  - SDA: GPIO 14
  - SCL: GPIO 15
- Microfone: Conectado via DMA
- Display: SSD1306 OLED (128x64 pixels)

## 💻 Dependências de Software
- Raspberry Pi Pico SDK
- Bibliotecas:
  - `microphone.h` (Desenvolvida por: Ivan da Silva Filho e Ygor)
  - `DisplaySSD1306.h` (Desenvolvida por: Ivan da Silva Filho e Ygor)
  - `Fonts.h` (Desenvolvida por: Ivan da Silva Filho e Ygor)
  - `PDSFunctions.h` (Desenvolvida para o Projeto)

## 🛠 Técnicas de Processamento de Sinais
- Taxa de amostragem: 5000 Hz
- Filtragem passa-baixa
- Transformada Rápida de Fourier (FFT) Radix-2
- Normalização de magnitude de frequência

## 📊 Características do Display
- Eixo X: Níveis de decibéis
- Eixo Y: Frequência em Hz
- Faixa de visualização: 80 Hz - 300 Hz
- Largura das barras: 2 pixels

## 🚧 Configuração e Instalação
1. Clone o repositório
2. Configure o Raspberry Pi Pico SDK
3. Configure seu ambiente de compilação (Importe como Pico Project pela extensão do VSCode)
4. Compile o projeto
5. Grave o firmware no seu Pico W

## 🔬 Personalização
- Ajuste `start_index` e `end_index` para modificar a faixa de frequência
- Altere `BAR_WIDTH` para modificar a resolução do display
- Modifique os coeficientes de filtro em `PDSFunctions.h`

## 📸 Exemplo de Saída
O display mostra um analisador de espectro em tempo real com barras verticais representando magnitudes de frequência na faixa de voz humana.