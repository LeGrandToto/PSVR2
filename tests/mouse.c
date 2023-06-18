// Compile with gcc mouse.c -o mouse -lusb-1.0
// You will probably need root privilege to for libusb to see all connected
// devices. libusb hides devices when claimed by the system (so if you can use
// your mouse, the mouse device is claimed by the system and regular users will
// not see it listed and this programm will fail)

#include <libusb-1.0/libusb.h>
#include <stdio.h>

#define VENDOR_ID 0x1ea7
#define PRODUCT_ID 0x0064

void handle_mouse_event(unsigned char button) {
  // Handle the button click event
  for (int i = 0; i < 8; ++i) {
    int value = button & (1 << i);
    if (value != 0) {
      printf("Button %d is pressed\n", i); // 0 -> left, 1 -> right, 2 -> middle
    }
  }
}

void reactivate_kernel_driver(libusb_device_handle *dev_handle) {
  printf("Reactivating kernel driver.\n");
  if (libusb_attach_kernel_driver(dev_handle, 0) != LIBUSB_SUCCESS) {
    printf("Failed to reactivate kernel driver.\n");
    return;
  }
  printf("Driver reactivated.\n");
}

int main() {
  // Initialize libusb
  libusb_context *ctx = NULL;
  int r = libusb_init(&ctx);
  if (r < 0) {
    printf("Failed to initialize libusb\n");
    return 1;
  }

  // Open the USB device with the given Vendor ID and Product ID
  libusb_device_handle *dev_handle =
      libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
  if (dev_handle == NULL) {
    printf("Failed to open the USB device\n");
    libusb_exit(ctx);
    return 1;
  }
  // Find the correct interface
  int interface_number = -1;
  int endpoint_number = -1;
  struct libusb_device_descriptor dev_desc;
  r = libusb_get_device_descriptor(libusb_get_device(dev_handle), &dev_desc);
  if (r == LIBUSB_SUCCESS) {
    for (int i = 0; i < dev_desc.bNumConfigurations; ++i) {
      struct libusb_config_descriptor *config;
      r = libusb_get_config_descriptor(libusb_get_device(dev_handle), i,
                                       &config);
      if (r == LIBUSB_SUCCESS) {
        for (int j = 0; j < config->bNumInterfaces; ++j) {
          const struct libusb_interface *iface = &config->interface[j];
          for (int k = 0; k < iface->num_altsetting; ++k) {
            const struct libusb_interface_descriptor *iface_desc =
                &iface->altsetting[k];
            if (iface_desc->bInterfaceClass == LIBUSB_CLASS_HID) {
              interface_number = iface_desc->bInterfaceNumber;
              endpoint_number = iface_desc->endpoint[0].bEndpointAddress;
              break;
            }
          }
          if (interface_number != -1)
            break;
        }
        libusb_free_config_descriptor(config);
      }
      if (interface_number != -1)
        break;
    }
  }

  if (interface_number == -1) {
    printf("Failed to find the mouse interface\n");
    libusb_close(dev_handle);
    libusb_exit(ctx);
    return 1;
  }

  printf("Interface number was %d\n", interface_number);
  printf("Endpoint address %d (0x%02x)\n", endpoint_number, endpoint_number);
  char reactivate_kernel = 1;
  if (libusb_kernel_driver_active(dev_handle, 0) != 0) {
    printf("Kernel driver is active!\n");
    printf("Detaching the kernel driver...\n");
    if (libusb_detach_kernel_driver(dev_handle, 0) != 0) {
      fprintf(stderr,
              "%s:%d: unable to detach the kernel driver from the interface "
              "of the USB device\n ",
              __FILE__, __LINE__);
      return -1;
    }
    printf("Done!\n");
    reactivate_kernel = 0;
  }

  // Claim the interface of the USB device
  r = libusb_claim_interface(dev_handle, interface_number);
  if (r < 0) {
    printf("Failed to claim the USB interface\n");
    if (reactivate_kernel == 0) {
      reactivate_kernel_driver(dev_handle);
    }

    libusb_close(dev_handle);
    libusb_exit(ctx);
    return 1;
  }

  // Set up an infinite loop to continuously monitor mouse events
  int tries = 0;
  while (tries < 10) {
    unsigned char data[8];
    int transferred;
    r = libusb_interrupt_transfer(dev_handle, endpoint_number, data,
                                  sizeof(data), &transferred, 5000);
    if (r == LIBUSB_SUCCESS) {
      // Process the mouse event data

      // printf("Probing endpoint was a success! %d bytes were "
      //        "transfered\n",
      //        transferred);
      for (int i = 0; i < transferred; ++i) {
        printf("%02x ", data[i]);
      }
      printf("\n");
      unsigned char button = data[1];
      handle_mouse_event(button);
    } else {
      printf("Transfer failed\n");
      printf("Transfer failed: %s\n", libusb_error_name(r));
      break;
    }
    ++tries;
  }
  printf("Out of the loop! Closing...\n");

  // Release the interface and close the device
  libusb_release_interface(dev_handle, 0);
  if (reactivate_kernel == 0) {
    reactivate_kernel_driver(dev_handle);
  }

  libusb_close(dev_handle);

  // Deinitialize libusb
  libusb_exit(ctx);

  return 0;
}
