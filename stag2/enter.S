.global main,old_sp,enter,stack
//int enter(int dram_size){
//return main(dram_size);
//}
enter:
    ldr r1, =old_sp
    str sp, [r1]
    ldr sp, =(stack+2044)
    mov r11,lr
    bl main
    mov lr,r11
    ldr r1, =old_sp
    ldr sp, [r1]
    bx lr
