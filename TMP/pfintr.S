/* pfintr.S - pfintr */


    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:
    popl pferrcode    /* Store error code in global variable pferrcode */
    pushfl            /* Store flag register */
    cli               /* Clear all Interrupts */
    pushal            /* Save all General Registers */
    call pfint        /* Calling page fault handler */
    popal             /* Restore General Registers */
    popfl             /* Restore Flag Registers */
    iret

