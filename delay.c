#include "stm32f10x.h"
#include "delay.h"
void Delay_ms(uint32_t milliseconds) {
    // Tính toán s? chu k? xung nh?p c?n ch?
    uint32_t cycles = milliseconds * (SystemCoreClock / 1000);

    // L?p d? t?o th?i gian ch?
    for (uint32_t i = 0; i < cycles; i++) {
        __NOP(); // No operation (optional)
    }
}
