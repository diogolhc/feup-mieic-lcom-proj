// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "kbc.h"
#include "keyboard.h"
#include "mouse.h"
#include "video_gr.h"
#include "rtc.h"
#include "canvas.h"
#include "cursor.h"
#include "font.h"
#include "dispatcher.h"
#include "textbox.h"
#include "game.h"
#include "menu.h"
#include "uart.h"
#include "protocol.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  //lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  //lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {
    uint16_t mode = 0x118; // 1024x768
    enum xpm_image_type image_type = XPM_8_8_8;
    uint8_t timer_irq_set, kbd_irq_set, mouse_irq_set, rtc_irq_set, com1_irq_set;
    rtc_interrupt_config_t rtc_periodic_config = {.periodic_RS3210 = 0x0f}; // period = 0.5 seconds

    // Video card

    if (vg_init(mode) == NULL) 
        return 1;

    // Timer

    if (timer_subscribe_int(&timer_irq_set) != OK) 
        return 1;

    // Keyboard

    if (kbd_subscribe_int(&kbd_irq_set) != OK) 
        return 1;

    // Mouse

    if (mouse_enable_dr() != OK)
        return 1;

    if (mouse_subscribe_int(&mouse_irq_set) != OK) 
        return 1;
    
    // RTC
    
    if (rtc_flush() != OK) // slide 23
        return 1;

    if (rtc_read_date() != OK) // to have the date ready since the first frame
        return 1;

    if (rtc_subscribe_int(&rtc_irq_set) != OK)
        return 1;
    
    if (rtc_enable_update_int() != OK)
        return 1;

    if (rtc_enable_int(PERIODIC_INTERRUPT, rtc_periodic_config) != OK)
        return 1;

    // UART and communication protocol

    if (protocol_config_uart() != OK)
        return 1;

    if (com1_subscribe_int(&com1_irq_set) != OK)
        return 1;
    
    uint8_t noop;
    if (uart_flush_received_bytes(&noop, &noop, &noop) != OK)
        return 1;    

    // Program assets and initializations
    // TODOPORVER probably move those to a more appropriate place later in the project
    srand(rtc_get_seed());
    protocol_send_program_opened();
    font_load(image_type); 
    game_load_assets(image_type);
    cursor_init(image_type);
    menu_init(image_type);
    if (menu_set_main_menu() != OK)
        return 1;
    // ^^

    int ipc_status, r;
    message msg;
    while ( !should_end() ) {
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
            printf("driver_receive failed with: %d\n", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & BIT(mouse_irq_set)) {
                    mouse_ih();
                    
                    if (mouse_is_packet_ready()) {
                        struct packet p;
                        if (mouse_retrieve_packet(&p) != OK) {
                            printf("mouse_retrieve_packet failed\n");

                        } else if (dispatch_mouse_packet(p) != OK) {
                            printf("dispatch_mouse_packet failed\n");
                        }
                    }
                }
                if (msg.m_notify.interrupts & BIT(kbd_irq_set)) {
                    kbc_ih();
                    
                    if (kbd_is_scancode_ready()) {
                        kbd_event_t kbd_state;
                        
                        if (kbd_handle_scancode(&kbd_state) != OK) {
                            printf("kbd_handle_scancode failed\n");

                        } else if (dispatch_keyboard_event(kbd_state) != OK) {
                            printf("dispatch_keyboard_event failed\n");
                        }
                    }
                }
                if (msg.m_notify.interrupts & BIT(rtc_irq_set)) {
                    rtc_ih();
                }
                if (msg.m_notify.interrupts & BIT(com1_irq_set)) {
                    com1_ih();
                    
                    if (protocol_handle_received_bytes() != OK) {
                        printf("Error handling received uart bytes.\n");
                    }

                    if (uart_error_reading_message()) {
                        if (protocol_handle_error() != OK) {
                            printf("Failed to handle uart error.\n");
                        }
                    }
                }
                if (msg.m_notify.interrupts & BIT(timer_irq_set)) {
                    timer_int_handler();
                    protocol_tick();
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    // EXIT game assets
    // TODOPORVER probably move those to a more appropriate place later in the project
    if (canvas_exit() != OK)
        return 1;
    if (text_box_clip_board_exit() != OK)
        return 1;
    cursor_exit();
    font_unload();
    game_unload_assets();
    menu_exit();
    dispatcher_bind_buttons(0);
    dispatcher_bind_text_boxes(0);
    dispatcher_bind_canvas(false);
    // ^^

    if (com1_unsubscribe_int() != OK)
        return 1;
    
    protocol_exit();
    
    if (rtc_disable_int(PERIODIC_INTERRUPT) != OK)
        return 1;

    if (rtc_disable_int(UPDATE_INTERRUPT) != OK)
        return 1;
    
    if (rtc_unsubscribe_int() != OK)
        return 1;
    
    if (kbd_unsubscribe_int() != OK)
        return 1;

    if (mouse_unsubscribe_int() != OK) 
        return 1;
    
    if (mouse_disable_dr() != OK) 
        return 1;
    
    if (kbc_flush() != OK)
        return 1;

    if (timer_unsubscribe_int() != OK)
        return 1;

    if (vg_exit() != OK)
        return 1;

    return 0;
}
