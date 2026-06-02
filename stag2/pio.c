#include "pio.h"
#define GET_FUNC_REG_INDX(pin) (pin>>3)
#define GET_FUNC_REG_SHFT(pin) ((pin&0x7)<<2)
#define GET_PLDR_REG_INDX(pin) (pin>>4)
#define GET_PLDR_REG_SHFT(pin) ((pin&0x3)<<1)
#define GET_PIN_BANK(bank) (SUNXI_PIO_CTRL->banks[(int)bank])
char gpio_get_pin_func(char bank,char pin){
 return (GET_PIN_BANK(bank).cfg[GET_FUNC_REG_INDX(pin)]>>GET_FUNC_REG_SHFT(pin))&0xf;
}
void gpio_set_pin_func(char bank,char pin,char func){
 GET_PIN_BANK(bank).cfg[GET_FUNC_REG_INDX(pin)]&=~(0xf<<GET_FUNC_REG_SHFT(pin));
 GET_PIN_BANK(bank).cfg[GET_FUNC_REG_INDX(pin)]|=func<<GET_FUNC_REG_SHFT(pin);
}
void gpio_set_pin_val(char bank,char pin,char val){
 if(val) GET_PIN_BANK(bank).dat|=1<<pin;
 else GET_PIN_BANK(bank).dat&=~(1<<pin);
}
char gpio_get_pin_val(char bank,char pin){
 return (GET_PIN_BANK(bank).dat>>pin)&1;
}
void gpio_set_pin_pull(char bank,char pin,char pull){
 GET_PIN_BANK(bank).pull[GET_PLDR_REG_INDX(pin)]&=~(0x3<<GET_PLDR_REG_SHFT(pin));
 GET_PIN_BANK(bank).pull[GET_PLDR_REG_INDX(pin)]|=pull<<GET_PLDR_REG_SHFT(pin);
}
char gpio_get_pin_pull(char bank,char pin){
 return (GET_PIN_BANK(bank).pull[GET_PLDR_REG_INDX(pin)]>>GET_PLDR_REG_SHFT(pin))&0x3;
}
void gpio_set_pin_drv(char bank,char pin,char drv){
 GET_PIN_BANK(bank).drv[GET_PLDR_REG_INDX(pin)]&=~(0x3<<GET_PLDR_REG_SHFT(pin));
 GET_PIN_BANK(bank).drv[GET_PLDR_REG_INDX(pin)]|=drv<<GET_PLDR_REG_SHFT(pin);
}
char gpio_get_pin_drv(char bank,char pin){
 return (GET_PIN_BANK(bank).drv[GET_PLDR_REG_INDX(pin)]>>GET_PLDR_REG_SHFT(pin))&0x3;
}
