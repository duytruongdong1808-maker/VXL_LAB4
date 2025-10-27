#include "input_reading.h"

GPIO_TypeDef* buttonPorts[N0_OF_BUTTONS] = {
    GPIOA, GPIOA, GPIOA
};

uint16_t buttonPins[N0_OF_BUTTONS] = {
    GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
};

static GPIO_PinState buttonBuffer[N0_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer1[N0_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer2[N0_OF_BUTTONS];

static uint8_t  flagForButtonPress1s[N0_OF_BUTTONS];
static uint32_t pressDurationMs[N0_OF_BUTTONS];
static uint8_t  buttonPressedFlag[N0_OF_BUTTONS];

void button_reading(uint32_t dt_ms)
{
    for (uint8_t i = 0; i < N0_OF_BUTTONS; i++)
    {
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(buttonPorts[i], buttonPins[i]);

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i])
        {
            if (buttonBuffer[i] != debounceButtonBuffer1[i])
            {
                buttonBuffer[i] = debounceButtonBuffer1[i];

                if (buttonBuffer[i] == BUTTON_IS_PRESSED)
                    buttonPressedFlag[i] = 1;

                pressDurationMs[i] = 0;
                flagForButtonPress1s[i] = 0;
            }

            if (buttonBuffer[i] == BUTTON_IS_PRESSED)
            {
                if (pressDurationMs[i] < 1000)
                {
                    pressDurationMs[i] += dt_ms;
                }
                else
                {
                    flagForButtonPress1s[i] = 1;
                }
            }
        }
    }
}

unsigned char is_button_pressed(uint8_t index)
{
    if (index >= N0_OF_BUTTONS) return 0;
    if (buttonPressedFlag[index]) {
        buttonPressedFlag[index] = 0;
        return 1;
    }
    return 0;
}

unsigned char is_button_pressed_1s(uint8_t index)
{
    if (index >= N0_OF_BUTTONS) return 0xff;
    if (flagForButtonPress1s[index] && buttonBuffer[index] == BUTTON_IS_PRESSED) {
        flagForButtonPress1s[index] = 0;
        return 1;
    }
    return 0;
}
