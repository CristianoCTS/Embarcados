# Introdução aos Sistemas Embarcados

Projetos e códigos fonte desenvolvidos na disciplina de **Introdução aos Sistemas Embarcados**, ministrada pelo professor Ricardo Pezzuol Jacobi no semestre 2025.2.

O repositório foca no desenvolvimento de software próximo ao hardware (Bare-metal/C) e controle de periféricos.

## Organização do Repositório:

- **`Trabalho_VGA`**: Implementação de controle de vídeo e geração de sinal VGA utilizando o kit de desenvolvimento.
- **`Trabalho_final`**: Projeto de encerramento da disciplina, integrando os diversos periféricos para desenvolver um jogo na placa DE1-SoC.

## 📚 Detalhamento Técnico

### 📂 Trabalho_VGA: Controle de Vídeo em Hardware
Implementação de um driver de vídeo "bare-metal" ou descrição de hardware (HDL) para interface VGA.
* **Timing:** Geração precisa dos pulsos de sincronismo vertical (VSync) e horizontal (HSync) baseada no clock do sistema (ex: 25MHz para 640x480).
* **Renderização:** Lógica de varredura de memória (framebuffer) ou geração procedimental de pixels para exibir padrões e caracteres no monitor, respeitando os tempos de *front porch* e *back porch*.

### 📂 Trabalho_final: Sistema Integrado
Projeto que une hardware e software para controle de periféricos.
* **Interrupções (ISR):** Uso de rotinas de serviço de interrupção para tratamento de eventos externos (botões, sensores) sem travar o processador (substituindo *polling*).
* **Comunicação:** Implementação de protocolos seriais (UART, I2C ou SPI) para comunicação com módulos externos (sensores de temperatura, acelerômetros ou displays LCD).
* **Arquitetura:** Organização do código em camadas de abstração de hardware (HAL) para facilitar a manutenção e portabilidade.
