#ifndef PIO_H
#define PIO_H
typedef unsigned int uint;
struct sunxi_gpio{
uint cfg[4];
uint dat;
uint drv[2];
uint pull[2];
};
struct sunxi_gpio_int{
uint cfg[3];
uint ctl;
uint sta;
uint deb;
};
struct sunxi_gpio_reg{
struct sunxi_gpio banks[9];
char PAD[188];
struct sunxi_gpio_int interrupt;
};
#define SUNXI_PIO_CTRL ((struct sunxi_gpio_reg*)0x01c20800)
void gpio_set_pin_func(char bank,char pin,char func);
void gpio_set_pin_val(char bank,char pin,char val);
void gpio_set_pin_pull(char bank,char pin,char pull);
void gpio_set_pin_drv(char bank,char pin,char drv);

char gpio_get_pin_func(char bank,char pin);
char gpio_get_pin_val(char bank,char pin);
char gpio_get_pin_pull(char bank,char pin);
char gpio_get_pin_drv(char bank,char pin);
#endif /* PIO_H */
