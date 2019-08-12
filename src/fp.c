    
#include <stdio.h>
#include <stdlib.h>
#include <startup.h>
#include <startup.c>
#include <inttypes.h>
#include "fp.h"
#include "pm64.h"
#include "gfx.h"


__attribute__((section(".data")))
fp_ctxt_t fp = { 
    .ready = 0, 
};

void fp_main(void){

    
    gfx_begin();
    gfx_printf(15,60,"%8x",pm_player.talkable_npc);



    /* draw input display */
    {
        input_t      input  = pm_status.input;
        controller_t pad    = input.raw;
        int16_t      pad_x  = input.control_x;
        int16_t      pad_y  = input.control_y;

        gfx_printf(16,240-16,"%4i %4i",pad_x,pad_y);
        gfx_printf_color(106,240-16,GPACK_RGBA8888(0x00,0x00,0xFF,0xFF),"%s", pad.a?"A":" ");
        gfx_printf_color(116,240-16,GPACK_RGBA8888(0x00,0xFF,0x00,0xFF),"%s", pad.b?"B":" ");
        gfx_printf_color(126,240-16,GPACK_RGBA8888(0xFF,0x00,0x00,0xFF),"%s", pad.s?"S":" ");
        gfx_printf_color(136,240-16,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),"%s%s%s", pad.z?"Z":" ",pad.l?"L":" ",pad.r?"R":" ");
        gfx_printf_color(166,240-16,GPACK_RGBA8888(0xFF,0xFF,0x00,0xFF),"%s%s%s%s", pad.cl?"<":" ",pad.cu?"^":" ", pad.cr?">":" ",pad.cd?"v":" ");
        gfx_printf_color(206,240-16,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),"%s%s%s%s", pad.dl?"<":" ",pad.du?"^":" ", pad.dr?">":" ",pad.dd?"v":" ");
        
    }
    

    gfx_finish();
    pm_GameUpdate(); /* displaced function call - advances 1 game frame*/
    

}

void init(){
    clear_bss();
    do_global_ctors();
    gfx_init();
    fp.ready = 1;

}

// Initilizes and uses new stack instead of using graph threads stack. 
static void init_stack(void (*func)(void)) {
    static _Alignas(8) __attribute__((section(".stack"))) 
    char stack[0x2000];
    __asm__ volatile(   "la     $t0, %1;"
                        "sw     $sp, -0x04($t0);"
                        "sw     $ra, -0x08($t0);"
                        "addiu  $sp, $t0, -0x08;"
                        "jalr   %0;"
                        "nop;"
                        "lw     $ra, 0($sp);"
                        "lw     $sp, 4($sp);"
                        ::
                        "r"(func),
                        "i"(&stack[sizeof(stack)]));
}


/* fp entry point - init stack and call main function */
ENTRY void _start(void){

    init_gp();
    if(!fp.ready){
        init_stack(init);
    }
    init_stack(fp_main);


}
