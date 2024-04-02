#include "HPS_PrivateTimer.h"
#include "Util/bit_helpers.h"

//Control Bit Map
#define TIMER_ENABLE      (1 << 0)  //Timer Enable Flag
#define TIMER_AUTO_RELOAD (1 << 1)  //Timer Auto Reload Flag
#define TIMER_IRQ_ENABLE  (1 << 2)  //Timer IRQ Enable Flag

//Register Offsets
#define TIMER_LOAD      (0x00/sizeof(unsigned int))
#define TIMER_VALUE     (0x04/sizeof(unsigned int))
#define TIMER_CONTROL   (0x08/sizeof(unsigned int))
#define TIMER_INTERRUPT (0x0C/sizeof(unsigned int))

// Cleanup function called when driver destroyed.
//  - Disables any hardware and interrupts.
//  - Free any allocated memory
static void _Timer_cleanup(PTimerCtx_t ctx) {
    if (ctx->base) {
        // Stop the timer running when driver cleared.
        ctx->base[TIMER_CONTROL] = 0;
    }
}

// Initialise Private Timer Driver
//  - base is a pointer to the private timer module
//  - Returns Util/error Code
//  - Returns context pointer to *ctx
HpsErr_t Timer_initialise(void* base, PTimerCtx_t* pCtx) {
    //Ensure user pointers valid
    if (!base) return ERR_NULLPTR;
    if (!pointerIsAligned(base, sizeof(unsigned int))) return ERR_ALIGNMENT;
    //Allocate the driver context, validating return value.
    HpsErr_t status = DriverContextAllocateWithCleanup(pCtx, &_Timer_cleanup);
    if (IS_ERROR(status)) return status;
    //Save base address pointers
    PTimerCtx_t ctx = *pCtx;
    ctx->base = (unsigned int*)base;
    //Ensure timer initialises to disabled
    ctx->base[TIMER_CONTROL] = 0;
    //Initialised
    DriverContextSetInit(ctx);
    return status;
}

// Check if driver initialised
//  - Returns true if driver previously initialised
bool Timer_isInitialised(PTimerCtx_t ctx) {
    return DriverContextCheckInit(ctx);
}

//Set Load value in the A9 Private Timer
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_setLoad( PTimerCtx_t ctx, unsigned int load) {
    //Ensure context valid and initialised and channel is valid
    HpsErr_t status = DriverContextValidate(ctx);
    if (IS_ERROR(status)) return status;

    //Configure Load
    ctx->base[TIMER_LOAD] = load; // This sets the load value, which in turn sets the current value reg.
    return ERR_SUCCESS;
}


//Set Prescaler value in the A9 Private Timer
// - "Prescaler" is the (clock divider-1).
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_setPrescaler( PTimerCtx_t ctx, unsigned char prescaler) {
    //Ensure context valid and initialised and channel is valid
    HpsErr_t status = DriverContextValidate(ctx);
    if (IS_ERROR(status)) return status;

    //Configure Prescaler
    ctx->base[TIMER_CONTROL] = MaskModify(ctx->base[TIMER_CONTROL], prescaler, 0xFF, 8);
    return ERR_SUCCESS;
}

//Set IRQ mode flag in the A9 Private Timer
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_setIRQ( PTimerCtx_t ctx, bool enable) {
    //Ensure context valid and initialised and channel is valid
    HpsErr_t status = DriverContextValidate(ctx);
    if (IS_ERROR(status)) return status;

    //Configure IRQ mode
    if (enable) {
        ctx->base[TIMER_CONTROL] |=  TIMER_IRQ_ENABLE; // Set individual bit
    } else {
        ctx->base[TIMER_CONTROL] &= ~TIMER_IRQ_ENABLE; // Clear individual bit
    }
    return ERR_SUCCESS;
}

//Set Auto Reload flag in the A9 Private Timer
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_setAutoReload( PTimerCtx_t ctx, bool enable) {
    //Ensure context valid and initialised and channel is valid
    HpsErr_t status = DriverContextValidate(ctx);
    if (IS_ERROR(status)) return status;

    //Configure Auto Reload
    if (enable) {
        ctx->base[TIMER_CONTROL] |=  TIMER_AUTO_RELOAD; // Set individual bit
    } else {
        ctx->base[TIMER_CONTROL] &= ~TIMER_AUTO_RELOAD; // Clear individual bit
    }
    return ERR_SUCCESS;
}

//Enable/Disable the A9 Private Timer
// - "set_enabled" is true to enable, false to disable.
// - returns ERR_SUCCESS if successful
HpsErr_t Timer_enable( PTimerCtx_t ctx, bool enable) {
    //Ensure context valid and initialised and channel is valid
    HpsErr_t status = DriverContextValidate(ctx);
    if (IS_ERROR(status)) return status;

    if (enable) {
        ctx->base[TIMER_CONTROL] |=  TIMER_ENABLE; // Set individual bit
    } else {
        ctx->base[TIMER_CONTROL] &= ~TIMER_ENABLE; // Clear individual bit
    }
    return ERR_SUCCESS;
}

//Read Back Current Value from A9 Timer
HpsErr_t Timer_currentValue( PTimerCtx_t ctx, unsigned int* value) {
    // Validate inputs
    if (!value) return ERR_NULLPTR;
    HpsErr_t status = DriverContextValidate(ctx);
    if (IS_ERROR(status)) return status;

    // Read the Current Timer Value.
    *value = ctx->base[TIMER_VALUE];
    return ERR_SUCCESS;
}

//Read Current Interrupt Flag and if set clear from A9 Timer
HpsErr_t Timer_irqFlag( PTimerCtx_t ctx, bool* irqFlag) {
    // Validate inputs
    if (!irqFlag) return ERR_NULLPTR;
    HpsErr_t status = DriverContextValidate(ctx);
    if (IS_ERROR(status)) return status;

    // Read the IRQ Flag
    *irqFlag = ctx->base[TIMER_INTERRUPT];
    // If set, clear the IRQ flag
    if(*irqFlag) ctx->base[TIMER_INTERRUPT] = 0x1;
    return ERR_SUCCESS;
}

//function that starts the timer
void Timer_start(PTimerCtx_t ctx) {
    ctx->base[TIMER_CONTROL] |= 1; // Start the timer
}

// function that stops the timer and calculates the elapsed time
unsigned int Timer_stop(PTimerCtx_t ctx) {
    unsigned int elapsedTime = ctx->base[TIMER_VALUE];
    ctx->base[TIMER_CONTROL] = 0; // Stop the timer
    return elapsedTime;
}
