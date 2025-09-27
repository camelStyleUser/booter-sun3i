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
