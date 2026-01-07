#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/dma.h"
#include "hardware/regs/rvcsr.h"
#include "Config/DEV_Config.h"
#include "lcd/LCD_1in28.h"
#include "lvgl/lvgl.h"

/* DISPLAY BUFFER OPTIONS: These are special in the sense, that we just want to
   test the Frames-per-second (FPS) in different scenarios here.

   In your normal program, we would choose 2 buffers with halve the
   screen-size, possible adding a third buffer for even more parallel rendering
   (rendering parallel to the DMA transfer).
   */
#define DISPLAY_BUFFER_DIVISOR 2    // This may be 1(?), 2, 3 or even up to 10, the smallest screen-fraction, recommended by LVGL-docu

#define WANT_2ND_DISPLAY_BUFFER     // Whether a 2nd draw buffer is to be registered (recommended): double buffering
// #define WANT_3RD_DISPLAY_BUFFER  // Whether a 3rd draw buffer is to be register (not recommened): triple buffering

/* THIS IS THE MOST IMPORTANT SETTING TO ACTIVATE */
// #define WANT_ANIMATION              // Whether we want a scale widet simulating the Gyroscope value of the Innertial Measurement Unit (IMU)

#define SLEEP_MS             20     // Time (in ms) to sleep (possibly putting the core to sleep)
#define FIB_DEPTH            32     // Number of Fibonacci to compute (works up to a value of 32)

/* Check the values provided */
#if !defined(DISPLAY_BUFFER_DIVISOR) || !((DISPLAY_BUFFER_DIVISOR >= 1) || (DISPLAY_BUFFER_DIVISOR <= 10) )
#error "You need to define DISPLAY_BUFFER_DIVISOR to 1, 2 up to 10."
#endif

#if !defined(WANT_2ND_DISPLAY_BUFFER) && defined(WANT_3RD_DISPLAY_BUFFER)
#error "Defining WANT_3RD_DISPLAY_BUFFER while not defining WANT_2ND_DISPLAY_BUFFER makes no sense."
#endif
/* END OF DISPLAY BUFFER OPTIONS */


#define STR1(x) #x
#define STRING(x)  STR1(x)

/********************************************************************************
 * Function definitions
 ********************************************************************************/
static uint32_t my_tick_cb(void);
static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void dma_handler_cb(void);

static uint32_t cpu_counter_init(void);
static inline uint64_t cpu_rdinstret(void);
static inline uint64_t cpu_rdcycles(void);

static void draw_table_event_cb(lv_event_t * e);
static void update_table_cb(lv_timer_t * timer);
#if defined(WANT_ANIMATION)
static void set_circle_value_cb(void * obj, int32_t v);
#endif

static void lvgl_init(void);
static lv_obj_t * widgets_init(void);

static uint32_t fib(uint32_t n);

/********************************************************************************
 * Global Variables 
 ********************************************************************************/
static lv_display_t * display;

static int dma_tx;
static dma_channel_config dma_channel_cfg;

/* To show animation */
#if defined(WANT_ANIMATION)
static lv_obj_t * circle;
static lv_anim_t circle_anim;
#endif

// The following global data is just for benchmarking
static int alg = 0;
static const char * const alg_string[] = {
        "none",
        "sleep_ms(" STRING(SLEEP_MS) ")",
        "fib(" STRING(FIB_DEPTH) ")"
    };
#define ALG_STRING_NUM  (sizeof(alg_string)/sizeof(alg_string[0]))
static int frames = 0;
static int time_ms_last;

#define ERROR(errno, text) do { \
        printf("ERROR: %s errno:%d\n", (text), (errno)); \
        exit((errno)); \
    } while (0)


/*
 * For LVGL this callback provides a sense of current time, required to
 * support in-time redraw of animations (every 33ms).
 */
static uint32_t my_tick_cb(void) {
    return to_ms_since_boot(get_absolute_time());
}


/*
 * For LVGL this callback writes the rendered buffer (in px_map) into the display.
 * First, we may have to tell the GC9A01A display controller, which memory location
 * in the display we write to.
 * If DISPLAY_BUFFER_DIVISOR==1 we do _not_ need to set this all over again,
 * otherwise, we set this using the LCD-Display driver function LCD_1IN28_SetWindows.
 * 
 * Then we copy into the Display buffer. If we don't have a 2nd (or even 3rd) Display-buffer,
 * we copy it (with the CPU!) using LCD-Display driver function LCD_1IN28_Display.
 * Otherwise, we may use the DMA functionality: the HW copies in the back-ground without
 * wasting CPU cycles. However, we require an additional callback: the dma_handler_cb()
 * below.
 */
#if !defined (WANT_2ND_DISPLAY_BUFFER)
static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map) {
#if DISPLAY_BUFFER_DIVISOR != 1
    /*
     * Write px_map to the rectangle (area->x1, area->y1 ; area->x2, area->y2)
     * in the external display controller.
     * We need to set the LCD controller (GC9A01A) to the area coordinates.
     */
    LCD_1IN28_SetWindows(area->x1, area->y1, area->x2, area->y2);
#endif
    LCD_1IN28_Display((uint16_t*) px_map);
    lv_display_flush_ready(display);
}
#else
static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map) {
#if DISPLAY_BUFFER_DIVISOR != 1
    /*
     * Write px_map to the rectangle (area->x1, area->y1 ; area->x2, area->y2)
     * in the external display controller.
     * We need to set the LCD controller (GC9A01A) to the area coordinates.
     * 
     * If however DISPLAY_BUFFER_DIVISOR == 1, no need to change the 
     */
    LCD_1IN28_SetWindows(area->x1, area->y1, area->x2, area->y2);
#endif
    /*
     * Start the data transfer to the LCD_SPI_PORT using DMA:
     * The format is RGB56, i.e. 2 Bytes per Pixel.
     */
    dma_channel_configure(dma_tx,
                          &dma_channel_cfg,
                          &spi_get_hw(LCD_SPI_PORT)->dr, 
                          px_map,
                          ((area->x2 - area->x1 +1)*(area->y2 - area->y1 +1))*2,
                          true);
}
#endif /* !WANT_2ND_DISPLAY_BUFFER */

/**
 * The dma_handler callback is called upon finished DMA transfer.
 * We need to acknowledge to the HW, that the next transfer may happen,
 * and need to tell LVGL that the buffer may be reused again.
 */
static void dma_handler_cb(void) {
    /* Did our DMA channel cause this IRQ0 interrupt? */
    if (dma_channel_get_irq0_status(dma_tx)) {
        /* Acknowledge this interrupt, resetting it */
        dma_channel_acknowledge_irq0(dma_tx);
        
        /* Just for Benchmarking, if this was the last frame of this, increment the frames just finished */
#if DISPLAY_BUFFER_DIVISOR == 1
        frames++;
#else
        if (lv_display_flush_is_last(display))
            frames++;
#endif

        /* Indicate to LVGL, that we are ready with this transfer, i.e. this buffer may be rendered-to again */
        lv_display_flush_ready(display);
    }
}

static void lvgl_init(void)
{
    /* Initialize LVGL, this has to be the first function call prior to any other LVGL function */
    lv_init();
    /* The proper way to allow LVGL's animations and screen-update information is to provide
     * a call-back function (cb) which returns time in milliseconds */
    lv_tick_set_cb(my_tick_cb);

    /* Initialize the Display with the correct resolution information in Pixels */
    display = lv_display_create(LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT);
    /* Set the Dots-per-Inch (DPI), sqrt(240^2 + 240^2)/1.28 inch; not really necessary */
    lv_display_set_dpi(display, 265);


    /* Set a call-back to flush the render buffer to the display, this is called every 33ms */
    lv_display_set_flush_cb(display, my_flush_cb);

    static uint16_t buf1[LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT / DISPLAY_BUFFER_DIVISOR];
#if defined(WANT_2ND_DISPLAY_BUFFER)
    static uint16_t buf2[LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT / DISPLAY_BUFFER_DIVISOR];
#else
    static uint16_t * buf2 = NULL;
#endif

#if DISPLAY_BUFFER_DIVISOR == 1
    lv_display_render_mode_t render_mode = LV_DISPLAY_RENDER_MODE_FULL;
#else
    lv_display_render_mode_t render_mode = LV_DISPLAY_RENDER_MODE_PARTIAL;
#endif

    /* Attach one or two buffers (each with either the full, halve or smaller screen size)
     * for full (divisor==1) or partial rendering (all other divisors) of the display in parallel */
    lv_display_set_buffers(display, buf1, buf2, sizeof(buf1), render_mode);

#ifdef WANT_3RD_DISPLAY_BUFFER
    LV_DRAW_BUF_DEFINE(buf3, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT/DISPLAY_BUFFER_DIVISOR, LV_COLOR_FORMAT_RGB565);
    lv_display_set_3rd_draw_buffer(display, &buf3);
#endif

    /* Register an unused channel for DMA; LVGL recommends high-priority DMA transfer */
    dma_tx = dma_claim_unused_channel(true);
    if (-1 == dma_tx)
        ERROR(-1, "dma_claim_unused_channel: returned -1");
    /* Reconfigure the DMA channel */
    dma_channel_cfg = dma_channel_get_default_config(dma_tx);
    /* Reconfigure the DMA channel, set to byte transfers -- word-transfers using DMA_SIZE_16 does not seem to work */
    channel_config_set_transfer_data_size(&dma_channel_cfg, DMA_SIZE_8);
    /* Set the transfer request signal to pace the transfer using this SPI-ports pacing function -- we are sending, i.e. true */
    channel_config_set_dreq(&dma_channel_cfg, spi_get_dreq(LCD_SPI_PORT, true));
    /* Enable this DMA channel using DMA_IRQ_0 */
    dma_channel_set_irq0_enabled(dma_tx, true);
    /*
     * Set the DMA handler for DMA_IRQ_0 exclusively: this disallows any other call-backs to be added to this DMA_IRQ_0.
     * This function sets the vector table shared by both cores. Since we only use one core, no need to synchronize.
     */
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler_cb);
    /* Enable the IRQ0 */
    irq_set_enabled(DMA_IRQ_0, true);
}


static void draw_table_event_cb(lv_event_t * e) {
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    /*If the cells are drawn...*/
    if(base_dsc->part == LV_PART_ITEMS) {
        uint32_t row = base_dsc->id1;
        uint32_t col = base_dsc->id2;

        /*In the first column align the texts to the right*/
        if(col == 0) {
            lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
            if(label_draw_dsc) {
                label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
            }
        }

        /*Make every 2nd row grayish*/
        if((row != 0 && row % 2) == 0) {
            lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if(fill_draw_dsc) {
                fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), fill_draw_dsc->color, LV_OPA_20);
                fill_draw_dsc->opa = LV_OPA_COVER;
            }
        }
    }
}

#if defined(WANT_ANIMATION)
static void set_circle_value_cb(void * obj, int32_t v) {
    // Getting the width of scale did not work...
    float g = ((float)v+90)/181;
    int32_t x = roundf(g * 190);  // On the left at -90, x=0, on the right at +90, x=162
    lv_obj_set_x(circle, x);
}
#endif


static lv_obj_t * widgets_init(void) {
    lv_obj_t * table = lv_table_create(lv_screen_active());

    /* Fill the first column */
    lv_table_set_cell_value(table, 0, 0, "Time(ms)");
    lv_table_set_cell_value(table, 1, 0, "Algorithm");
    lv_table_set_cell_value(table, 2, 0, "Instructions");
    lv_table_set_cell_value(table, 3, 0, "Cycles");
    lv_table_set_cell_value(table, 4, 0, "Instr./Cycle");
    lv_table_set_cell_value(table, 5, 0, "FPS");

    /* Fill the second column */
    lv_table_set_cell_value(table, 0, 1, "0");
    lv_table_set_cell_value(table, 1, 1, "Initializing");
    lv_table_set_cell_value(table, 2, 1, "0");
    lv_table_set_cell_value(table, 3, 1, "0");
    lv_table_set_cell_value(table, 4, 1, "0");
    lv_table_set_cell_value(table, 5, 1, "0");

    lv_table_set_cell_ctrl(table, 0, 1, LV_TABLE_CELL_CTRL_TEXT_CROP);
    lv_table_set_cell_ctrl(table, 1, 1, LV_TABLE_CELL_CTRL_TEXT_CROP);
    lv_table_set_cell_ctrl(table, 2, 1, LV_TABLE_CELL_CTRL_TEXT_CROP);
    lv_table_set_cell_ctrl(table, 3, 1, LV_TABLE_CELL_CTRL_TEXT_CROP);
    lv_table_set_cell_ctrl(table, 4, 1, LV_TABLE_CELL_CTRL_TEXT_CROP);
    lv_table_set_cell_ctrl(table, 5, 1, LV_TABLE_CELL_CTRL_TEXT_CROP);
    lv_table_set_column_width(table, 0, 100);
    lv_table_set_column_width(table, 1, 100);
    
    /* Add an event callback to to apply some custom drawing */
    lv_obj_add_event_cb(table, draw_table_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    /* Change the layout a bit: move more to top (20 pixels from the top)*/
    lv_obj_set_align(table, LV_ALIGN_TOP_MID);
    lv_obj_set_y(table, 15);
    lv_obj_set_scrollbar_mode(table, LV_SCROLLBAR_MODE_OFF);

    /* Change the style of the Cells(!) (here LV_PART_ITEMS), the padding, border-width and margin where too wide */
    lv_style_t table_style;
    lv_style_init(&table_style);
    // lv_style_set_height(&table_style, LV_SIZE_CONTENT);
    lv_style_set_pad_all(&table_style, 2);
    lv_style_set_border_width(&table_style, 1);
    lv_style_set_margin_all(&table_style, 3);
    lv_obj_add_style(table, &table_style, LV_PART_ITEMS|LV_STATE_DEFAULT); // Add the style for the ITEMS

#if defined(WANT_ANIMATION)
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_set_align(scale, LV_ALIGN_CENTER);
    lv_obj_set_width(scale, 190);
    lv_obj_set_y(scale, 95);
    lv_obj_set_style_length(scale, 4, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 8, LV_PART_INDICATOR);
    lv_scale_set_label_show(scale, true);
    lv_scale_set_total_tick_count(scale, 31);
    lv_scale_set_major_tick_every(scale, 5);
    lv_scale_set_range(scale, -90, 90);

    // Unfortunately, we cannot get the width, it seems: it's always zero!
    int scale_width = lv_obj_get_width(scale);
    printf("scale_width:%d\n", scale_width);

    circle = lv_obj_create(scale);
    lv_obj_set_style_bg_color(circle, lv_color_make(0xff, 0x0f, 0x0f), LV_STATE_DEFAULT);
    lv_obj_set_size(circle, 9, 9);
    lv_obj_set_pos(circle, 0, 1);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
    
    lv_anim_init(&circle_anim);
    lv_anim_set_var(&circle_anim, scale);
    lv_anim_set_exec_cb(&circle_anim, set_circle_value_cb);
    lv_anim_set_duration(&circle_anim, 1000);
    lv_anim_set_repeat_count(&circle_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_reverse_duration(&circle_anim, 1000);
    lv_anim_set_values(&circle_anim, -90, 90);
    lv_anim_start(&circle_anim);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Now FPS changes, also with\nmeans to display (2nd buffer)");
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(label, -25);
#else
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Take FPS lightly :) LVGL only\nredraws when needed.\nBut watch Instr. per cycle.\n   Set WANT_ANIMATION");
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(label, -40);
#endif
    return table;
}

/**
 * \brief Initialize the sampling of Instruction and Cycle counters.
 *
 * We want to sample the number of Instructions and the number of Cycles executed
 * every 500ms in sample_values_cb().
 * First we need to initialize the RISC-V Hardware Thread "hart" to NOT inhibit
 * these two counters to be counted, aka, we need to clear Bit 0 and Bit 2
 * (see PICO-datasheet page 313)
 * 
 * \return The previously set value of the CSR MCOUNTINHIBIT.
 */
static uint32_t cpu_counter_init(void) {
    uint32_t csr_value;
    /*
     * We need to access the CSR (ext. Zicsr) using the read-clear-immedate
     * instruction, passing the Offset 0x320 (defined in hardware/regs/rvcsr.h)
     * and the bits to sets both as immediate as input.
     * We as well may read the currently set value into the variable csr_value.
     * If we had a longer assembler sequence, we might need to inform GCC of any
     * "clobbered" (killed) resources, e.g. a register like "%t0" or "memory".
     */
    asm volatile ("csrrci %0, %1, %2\n\t"         // We add new-line and tab to beautify
                  : /* Output*/ "=r" (csr_value)  // The compiler replaces %0 as general register
                  : /* Input */"i" (RVCSR_MCOUNTINHIBIT_OFFSET), // The compiler replaces %1
                               "i" (0x1 << 0 | 0x1 << 2)         // and %2 as immediate values.
                  : /* No Clobber */ );
    return csr_value;
}

/**
 * \brief Return the number of retired instructions.
 * This requires cpu_counter_init() to be called
 * 
 * \return The number of instructions as 64-bit unsigned integer since boot.
 */
static inline uint64_t cpu_rdinstret(void) {
    uint32_t instr_l, instr_h;
    /*
     * Read the low and high 32-bits of the number of Instructions retired.
     * This is then combined to a 64-bit value (or better yet passed as two
     * registers to snprintf).
     * 
     * In order to fetch the (very unlikely) case, that the lower 32-bit overflow
     * (into the higher 32-bits), we read out the high-value twice, and repeat
     * the process, if the values are not equal.
     * We need to tell the compiler, that the register t0 is clobbered, using
     * gcc's way of specifying a register, memory is not touched, hence no
     * clobber of "memory"
     */
    asm volatile ("asm_rdinstret_again:\n\t"
                  "rdinstreth t0\n\t"          // Instead of letting the compiler choose,
                  "rdinstret  %0\n\t"          // we use the temporary register t0.
                  "rdinstreth %1\n\t"          // Hence, we need to mark it clobbered.
                  "bne        t0, %1, asm_rdinstret_again\n\t"
                  : /* Output */"=r" (instr_l), "=r" (instr_h)
                  : /* No Input */
                  : /* Clobber */ "%t0");
    return ((uint64_t)instr_h << 32) | instr_l;
}

/**
 * \brief Return the number of CPU cycles (of the core)
 * This requires cpu_counter_init() to be called
 * 
 * \return The number of cycles as 64-bit unsigned integer since boot.
 */
static inline uint64_t cpu_rdcycles(void) {
    uint32_t cycles_l, cycles_h;
    asm volatile ("asm_rdcycle_again:\n\t"
                  "rdcycleh t0\n\t"
                  "rdcycle  %0\n\t"
                  "rdcycleh %1\n\t"
                  "bne      t0, %1, asm_rdcycle_again\n\t"
                  : /* Output */"=r" (cycles_l), "=r" (cycles_h)
                  : /* No Input */
                  : /* Clobber */ "%t0");
    return ((uint64_t)cycles_h << 32) | cycles_l;
}

/**
 * \brief Callback function to update the table's values
 * 
 * This callback is called seldomly (once or twice a second), it updates the table with:
 * - the number of milliseconds (since boot),
 * - the "algorithm" currently executing
 * - the number of instructions executed (since the last update)
 * - the number of cycles executed (since the last update)
 * - the instructions per cycle (with RISC-V Hazard3 cores, the max is 1.0)
 * 
 * \param   timer   Used to grep the pointer to the table.
 */
static void update_table_cb(lv_timer_t * timer) {
    char string[16];
    lv_obj_t * table = lv_timer_get_user_data(timer);

    /* In the following, use lv_table_set_cell_value() instead of
     * lv_table_set_cell_value_fmt(), since it always will re-allocate
     * the memory.
     * We just print into the 16-BYte String.
     */

    int time_ms = to_ms_since_boot(get_absolute_time());
    snprintf(string, sizeof(string), "%d", time_ms);
    lv_table_set_cell_value(table, 0, 1, string);

    lv_table_set_cell_value(table, 1, 1, alg_string[alg]);

    uint64_t temp;
    static uint64_t instructions_last = 0;
    uint64_t instr = cpu_rdinstret();
    temp = instr;
    instr = instr - instructions_last;
    instructions_last = temp;
    snprintf(string, sizeof(string), "%llu", instr);
    lv_table_set_cell_value(table, 2, 1, string);


    static uint64_t cycles_last = 0;
    uint64_t cycles = cpu_rdcycles();
    temp = cycles;
    cycles = cycles - cycles_last;
    cycles_last = temp;
    snprintf(string, sizeof(string), "%llu", cycles);
    lv_table_set_cell_value(table, 3, 1, string);

    /*
     * Print the derived value Instructions retired per Clock-Cycles executed.
     * Modern micro-processors (like Intel & AMD) offer multiple execution "ports",
     * with multiple ALUs and even vector instructions, where multiple instructions
     * may be executed per clock-cycle.
     * The Hazard3 RISC-V is a typical MCU, offering only a 3-stage Pipeline with single-instruction
     * execution, so we may get at max 1 instruction per cycle -- here often way less.
     */
    snprintf(string, sizeof(string), "%1.4f", (float)instr/cycles);
    lv_table_set_cell_value(table, 4, 1, string);

    snprintf(string, sizeof(string), "%2.3f", 1000.0 * frames/(time_ms - time_ms_last));
    lv_table_set_cell_value(table, 5, 1, string);

    /* Reset the state for FPS and the timing information */
    time_ms_last = time_ms;
    frames = 0;
}

/**
 * \brief Calculate Fibonacci recursively
 * This function using recursive calling to calculate Fibonacci numbers is just plain _stupid_,
 * But wastes compute cycles with "high effiency": many instructions per clock-cycle.
 * 
 * \param n     With higher n, the execution time will grow exponenitally, 32 is still fine!
 */
static uint32_t fib(uint32_t n) {
    if (n <= 2)
        return 1;
    return fib(n - 1) + fib(n-2);
}

int main(void) {
    int ret;
    /* Initialize the GPIO for Display and PWM */
    if (0 != (ret = DEV_Module_Init()))
        ERROR(ret, "DEV_Module_Init()");
    
    /* First initialize the LCD (LVGL needs to know of a initialized "HW Display") */
    LCD_1IN28_Init(HORIZONTAL);
#if DISPLAY_BUFFER_DIVISOR == 1
    LCD_1IN28_SetWindows(0, 0, LCD_1IN28_WIDTH-1, LCD_1IN28_HEIGHT-1);
#endif
    
    /* Now initialize LVGL and draw a few Widgets*/
    lvgl_init();
    lv_obj_t * table = widgets_init();

    cpu_counter_init();    

    // Update the table every 1000ms
    // IMPORTANT: Only LVGL - callbacks should do lv_-rendering calls, otherwise the following will happen:
    // LV_ASSERT_MSG(!disp->rendering_in_progress, "Invalidate area is not allowed during rendering.")
    lv_timer_create(update_table_cb, 1000, table);

    int last_time_ms = to_ms_since_boot(get_absolute_time());
    lv_table_set_cell_value(table, 1, 1, alg_string[alg]);
    while (1) {
        lv_timer_handler();

        // Either do nothing, sleep (this may enter the power-safing state using WFI) or create a considerable computational load.
        switch (alg) {
            case 0:                   // Nothing, just fall through
                break;
            case 1:
                sleep_ms(SLEEP_MS);   // Enough time to still produce images ("halve-images") with 200 Hz / 100 Hz
                break;
            case 2:
                fib(FIB_DEPTH);
                break;
        }

        int time = to_ms_since_boot(get_absolute_time());
        // Every 5 seconds (5000 ms), change the algorithm
        if (time > (last_time_ms + 5000)) {
            last_time_ms = time;
            alg = (alg + 1) % ALG_STRING_NUM;
        }
    }

    DEV_Module_Exit();
    return 0;
}
