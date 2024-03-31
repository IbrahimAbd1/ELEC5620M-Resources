/* ARM Private Timer Driver
 * ------------------------
 *
 * Author: David Cowell
 * Date:   07/03/2024
 */

#ifndef HPS_PRIVATETIMER_H_
#define HPS_PRIVATETIMER_H_

#include "Util/driver_ctx.h"

// Driver context
typedef struct {
    //Header
    DrvCtx_t header;
    //Body
    volatile unsigned int * base;
    // ... 
} TimerCtx_t, *PTimerCtx_t;

// Initialise Private Timer Driver
//  - base is a pointer to the private timer module
//  - Returns Util/error Code
//  - Returns context pointer to *ctx
HpsErr_t Timer_initialise(void* base, PTimerCtx_t* pCtx);

// Check if driver initialised
//  - Returns true if driver previously initialised
bool Timer_isInitialised(PTimerCtx_t ctx);

//Set Load value in the A9 Private Timer
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_setLoad( PTimerCtx_t ctx, unsigned int load);

//Set Prescaler value in the A9 Private Timer
// - "Prescaler" is the (clock divider-1).
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_setPrescaler( PTimerCtx_t ctx, unsigned char prescaler);

//Set IRQ mode flag in the A9 Private Timer
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_Timer_setIRQ( PTimerCtx_t ctx, bool enable);

//Set Auto Reload flag in the A9 Private Timer
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_setAutoReload( PTimerCtx_t ctx, bool enable);

//Enable/Disable a A9 Private Timer
// - "set_enabled" is true to enable, false to disable.
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_enable( PTimerCtx_t ctx, bool enable );

//Read Back Current Value from A9 Timer
HpsErr_t Timer_currentValue( PTimerCtx_t ctx, unsigned int* value);

//Read Current Interrupt Flag and if set clear from A9 Timer
HpsErr_t Timer_irqFlag( PTimerCtx_t ctx, bool* irqFlag);

#endif /* HPS_PRIVATETIMER_H_ */
