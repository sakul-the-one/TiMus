/*
 *--------------------------------------
 * Program Name: WhatsApp
 * Author: now sakul
 * License: Use at own Risk
 * Description: Hippity Hoppity
 *--------------------------------------
*/
#include "Graphics/Graphicsfunc.h"
#include <srldrvce.h>

#include <debug.h>
#include <keypadc.h>
#include <stdbool.h>
#include <string.h>
#include <tice.h>
#include <ti/screen.h>
#include <ti/getcsc.h>

srl_device_t srl;

bool has_srl_device = false;

uint8_t srl_buf[512];

static usb_error_t handle_usb_event(usb_event_t event, void *event_data,
                                    usb_callback_data_t *callback_data __attribute__((unused))) {
    usb_error_t err;
    /* Delegate to srl USB callback */
    if ((err = srl_UsbEventCallback(event, event_data, callback_data)) != USB_SUCCESS)
        return err;
    /* Enable newly connected devices */
    if (event == USB_DEVICE_CONNECTED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE)) {
        usb_device_t device = event_data;
        GetMessage("Calc: device connected", 23);
        usb_ResetDevice(device);
    }

    /* Call srl_Open on newly enabled device, if there is not currently a serial device in use */
    if (event == USB_HOST_CONFIGURE_EVENT || (event == USB_DEVICE_ENABLED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE))) {

        /* If we already have a serial device, ignore the new one */
        if (has_srl_device) return USB_SUCCESS;

        usb_device_t device;
        if (event == USB_HOST_CONFIGURE_EVENT) {
            /* Use the device representing the USB host. */
            device = usb_FindDevice(NULL, NULL, USB_SKIP_HUBS);
            if (device == NULL) return USB_SUCCESS;
        } else {
            /* Use the newly enabled device */
            device = event_data;
        }

        /* Initialize the serial library with the newly attached device */
        srl_error_t error = srl_Open(&srl, device, srl_buf, sizeof srl_buf, SRL_INTERFACE_ANY, 9600);
        if (error) {
            /* Print the error code to the homescreen */
            GetMessage("Calc: Error initting serial", 28);
            return USB_SUCCESS;
        }

        GetMessage("Calc: serial initialized", 25);
        //srl_Write(&srl, "Hello wrold Computer!", 22);
        has_srl_device = true;
    }

    if (event == USB_DEVICE_DISCONNECTED_EVENT) {
        usb_device_t device = event_data;
        if (device == srl.dev) {
            GetMessage("Calc: device disconnected", 20);
            srl_Close(&srl);
            has_srl_device = false;
        }
    }

    return USB_SUCCESS;
}

int main(void) {
    uint8_t key = 0;
    os_ClrHome();
    //Init GraphicsUI
    char* buf = malloc(4);
    os_PutStrFull("Enter name (Max Chars: 3)");
    os_NewLine();
    os_GetStringInput("Name:", buf, 4);
    buf[3] = ':';
    Init(buf);
    free(buf);
    //Handle USB:
    ///*
    const usb_standard_descriptors_t *desc = srl_GetCDCStandardDescriptors();
    //Initialize the USB driver with our event handler and the serial device descriptors 
    usb_error_t usb_error = usb_Init(handle_usb_event, NULL, desc, USB_DEFAULT_INIT_FLAGS);
    if (usb_error) {
       GetMessage("Calc: usb init error", 21);
       do kb_Scan(); while(!kb_IsDown(kb_KeyClear));
       goto CriticError;
    } //*/
    key = os_GetCSC();
    GetMessage("Calc: Hello world!", 19);
    while (key != sk_Clear) 
    {
        key = os_GetCSC();
        char * send = NULL;
        send = HandleKeyBoard(key);

        usb_HandleEvents();

        if (has_srl_device) {
            char in_buf[128]; //32 Chars per person... Raspberry has only 4 Ports -> Max 4 persons
            //srl_Write(&srl, "Hello wrold Computer!", 22);
            //Read up to ~~64~~ 128 bytes from the serial buffer //
            size_t bytes_read = srl_Read(&srl, in_buf, sizeof in_buf);

            // Check for an error (e.g. device disconneced) //
            if (bytes_read < 0) {
                GetMessage("calc: error on srl_Read\n", 25);
                has_srl_device = false;
            } else if (bytes_read > 0) { 
                // Write the data back to serial //
                //os_PutStrFull(&in_buf);
                GetMessage(in_buf, sizeof in_buf);
                //srl_Write(&srl, in_buf, bytes_read);
            }
            //we got a message to send:
            if (send != NULL) 
            {
                //GetMessage("dbg: Message send!", 19);
                srl_Write(&srl, send, 32);
                //send[0] = 'd';
                //send[1] = 'b';
                //send[2] = 'g';
                //GetMessage(send, 32);
            }
            
        }
    }
    /*
    do {
        kb_Scan();

        usb_HandleEvents();

        if (has_srl_device) {
            char in_buf[64];

            //Read up to 64 bytes from the serial buffer //
            size_t bytes_read = srl_Read(&srl, in_buf, sizeof in_buf);

            // Check for an error (e.g. device disconneced) //
            if (bytes_read < 0) {
                os_PutStrFull("error  on srl_Read\n");
                has_srl_device = false;
            } else if (bytes_read > 0) {
                // Write the data back to serial //
                os_PutStrFull(&in_buf);
                srl_Write(&srl, in_buf, bytes_read);
            }
        }

    } while(!kb_IsDown(kb_KeyClear));*/
    
    usb_Cleanup();
    Close();
    return 0;
CriticError:
    usb_Cleanup();
    Close();
    return 1;
}