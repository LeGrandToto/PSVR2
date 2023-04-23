#include <libusb-1.0/libusb.h>
#include <stdint.h>
#include <stdio.h>

#define SONY_VENDOR_ID 0x054c
#define RIGHT_SENSE_CONTROLLER_DEVICE_ID 0x0e46
#define LEFT_SENSE_CONTROLLER_DEVICE_ID 0x0e45
#define PSVR2_DEVICE_ID 0x0cde

typedef unsigned short word;

union converter_type {
  word value;
  unsigned char bytes[2];
};

word convert_word(const word little_endian_word) {
  union converter_type converter;
  converter.value = little_endian_word;
  return (converter.bytes[0] << 8) + converter.bytes[1];
  return ((little_endian_word & 0x00FF) << 8) |
         ((little_endian_word & 0xFF00) >> 8);
}

void display_endpoint_address(const unsigned char address) {
  unsigned char endpoint_number = address & 0x0F;
  printf("%s #%d", address > 127 ? "IN" : "OUT", endpoint_number);
}

void display_endpoint_attributes(const unsigned char attribute) {
  unsigned char type = attribute & 0x03;
  printf(", ");
  switch (type) {
  case 0:
    printf("CONTROL");
    break;
  case 1:
    printf("ISOCHRONOUS ");
    unsigned char synchronisation_type = (attribute & 0x0C) >> 2;
    switch (synchronisation_type) {
    case 0:
      printf("No Synchronisation");
      break;
    case 1:
      printf("Asynchronous");
      break;
    case 2:
      printf("Adaptive");
      break;
    case 3:
      printf("Synchronous");
    }
    unsigned char data_type = (attribute & 0x30) >> 4;
    printf(" ");
    switch (data_type) {
    case 0:
      printf("Data endpoint");
      break;
    case 1:
      printf("Feedback endpoint");
      break;
    case 2:
      printf("Implicit feedback Data endpoint");
      break;
    case 3:
      printf("RESERVED");
    }
    break;
  case 2:
    printf("BULK");
    break;
  case 3:
    printf("INTERRUPT");
    unsigned char interrupt_type = attribute & 0x30;
    printf(interrupt_type > 1      ? " RESERVED"
           : (interrupt_type == 1) ? " Notification"
                                   : " Periodic");
  }
}

void print_strinf_from_index(libusb_device_handle *dev_handle,
                             unsigned char index) {
  char buf[256];
  int r = libusb_get_string_descriptor_ascii(dev_handle, index,
                                             (unsigned char *)buf, sizeof(buf));
  if (r > 0) {
    printf(" %s", buf);
  }
}

int main(int argc, char *argv[]) {
  // printf("Hello world\n");
  // return 0;
  libusb_device **dev_list;
  libusb_context *ctx;
  libusb_device_handle *dev_handle;
  struct libusb_device_descriptor dev_desc;

  int r, i;
  ssize_t cnt;

  // Initialize libusb
  r = libusb_init(&ctx);
  if (r < 0) {
    printf("Error initializing libusb: %s\n", libusb_strerror(r));
    return 1;
  }

  // Get the list of USB devices
  cnt = libusb_get_device_list(ctx, &dev_list);
  if (cnt < 0) {
    printf("Error getting device list: %s\n", libusb_strerror(cnt));
    libusb_exit(ctx);
    return 1;
  }

  // Loop through the devices and print their descriptors
  for (i = 0; i < cnt; i++) {
    libusb_device *dev = dev_list[i];
    r = libusb_open(dev, &dev_handle);
    if (r == LIBUSB_SUCCESS) {
      printf("Openning device %d\n", i);
      // Display the device descriptor
      r = libusb_get_device_descriptor(dev, &dev_desc);
      if (r == LIBUSB_SUCCESS) {
        printf(" Device Descriptor: (Length: %d)\n", dev_desc.bLength);
        printf("  Vendor ID: 0x%04x\n", dev_desc.idVendor);
        printf("  Product ID: 0x%04x\n", dev_desc.idProduct);
        printf("  USB Class: 0x%02x\n", dev_desc.bDeviceClass);
        printf("  USB Subclass: 0x%02x\n", dev_desc.bDeviceSubClass);
        printf("  USB Protocol: 0x%02x\n", dev_desc.bDeviceProtocol);
        printf("  Max Packet Size 0: 0x%02x\n", dev_desc.bMaxPacketSize0);
        printf("  Number of possible configurations: %d\n",
               dev_desc.bNumConfigurations);
        printf("  Manufacturer string index: %d\n", dev_desc.iManufacturer);
        printf("  Product string index: %d\n", dev_desc.iProduct);
        printf("  Serial Number string index: %d\n", dev_desc.iSerialNumber);
        printf("  bcdUSB: 0x%04x\n", dev_desc.bcdUSB);
        printf("  bcdDevice: 0x%04x\n", dev_desc.bcdDevice);

        char buf[256];
        r = libusb_get_string_descriptor_ascii(
            dev_handle, dev_desc.iManufacturer, (unsigned char *)buf,
            sizeof(buf));
        if (r >= 0) {
          printf("  Manufacturer: %s\n", buf);
        }

        r = libusb_get_string_descriptor_ascii(
            dev_handle, dev_desc.iProduct, (unsigned char *)buf, sizeof(buf));
        if (r >= 0) {
          printf("  Product: %s\n", buf);
        }

        r = libusb_get_string_descriptor_ascii(
            dev_handle, dev_desc.iSerialNumber, (unsigned char *)buf,
            sizeof(buf));
        if (r >= 0) {
          printf("  Serial Number: %s\n", buf);
        }

        if (dev_desc.idVendor == SONY_VENDOR_ID) {
          switch (dev_desc.idProduct) {
          case RIGHT_SENSE_CONTROLLER_DEVICE_ID:
            printf("  Sony Corp. PlayStation VR2 Sense Controller (R)\n");
            break;
          case LEFT_SENSE_CONTROLLER_DEVICE_ID:
            printf("  Sony Corp. PlayStation VR2 Sense Controller (L)\n");
            break;
          case PSVR2_DEVICE_ID:
            printf("  Sony Corp. BillBoard Device\n");
            break;
          }
        }
        // // list all string available on the device.
        // for (uint8_t i = 0; i < 255; ++i) {
        //   r = libusb_get_string_descriptor_ascii(
        //       dev_handle, i, (unsigned char *)buf, sizeof(buf));
        //   if (r >= 0) {
        //     printf("%i : %s\n", i, buf);
        //   }
        // }
        switch (dev_desc.bDeviceClass) {
        case LIBUSB_CLASS_HUB:
          printf("HUB!!!\n");
          break;
        case LIBUSB_CLASS_VENDOR_SPEC:
          printf("Vendor Specific!!!\n");
          break;
        }
      }

      // List the device's config descriptor
      struct libusb_config_descriptor *config;
      r = libusb_get_config_descriptor(dev, 0, &config);
      int num_endpoints = 0;
      if (r >= 0) {
        printf(" Config Descriptor:\n");
        printf("   Number of interfaces: %d", config->bNumInterfaces);
        print_strinf_from_index(dev_handle, config->iConfiguration);
        printf("\n");
        if (config->extra_length > 0) {
          printf(" Extra Length: %d\n", config->extra_length);
          for (int k = 0; k < config->extra_length; ++k) {
            printf("   %d", config->extra[k]);
          }
        }

        printf("    Attribute: %d (Self-Powered %d, Remote Wakeup %d) \n",
               config->bmAttributes, (config->bmAttributes & 0x20),
               (config->bmAttributes & 0x10));
        printf("    Max Power: %dmA (SuperSpeed mode: %dmA)\n",
               config->MaxPower * 2, config->MaxPower * 8);
        for (int i = 0; i < config->bNumInterfaces; ++i) {
          const struct libusb_interface *interface = &config->interface[i];
          for (int j = 0; j < interface->num_altsetting; ++j) {
            const struct libusb_interface_descriptor *altsetting =
                &interface->altsetting[j];
            printf(
                "\n    Interface #%d: Length:%d Type:%d Alternate Setting: %d "
                "Class: %d SubClass: %d Protocol: %d SDesc.: %d",
                altsetting->bInterfaceNumber, altsetting->bLength,
                altsetting->bDescriptorType, altsetting->bAlternateSetting,
                altsetting->bInterfaceClass, altsetting->bInterfaceSubClass,
                altsetting->bInterfaceProtocol, altsetting->iInterface);
            if (altsetting->extra_length > 0) {
              printf(" Extra Length: %d\n", altsetting->extra_length);
              for (int k = 0; k < altsetting->extra_length; ++k) {
                printf("   %d", altsetting->extra[k]);
              }
              printf("\n");
            }
            for (int k = 0; k < altsetting->bNumEndpoints; ++k) {
              const struct libusb_endpoint_descriptor *endpoint =
                  &altsetting->endpoint[k];

              printf("     Endpoint %d: ", num_endpoints++);

              display_endpoint_address(endpoint->bEndpointAddress);
              display_endpoint_attributes(endpoint->bmAttributes);

              printf(", Maximum Packet Size=%d, Extra Length=%d (i:%d, j:%d, "
                     "k:%d)\n",
                     // endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK,
                     convert_word(endpoint->wMaxPacketSize),
                     endpoint->extra_length, i, j, k);
            }
          }
        }
      }
      // List the device's bos configuration
      struct libusb_bos_descriptor *bos;
      r = libusb_get_bos_descriptor(dev_handle, &bos);
      if (r == LIBUSB_SUCCESS) {
        printf(" BOS Descriptor:\n");
        printf("  Device %d has BOS descriptor:\n", i);
        printf("  Length: %d\n", bos->bLength);
        printf("  Descriptor Type: %d\n", bos->bDescriptorType);
        printf("  Total Length: %d\n", convert_word(bos->wTotalLength));
        printf("  Number of device capabilities: %d\n", bos->bNumDeviceCaps);
        for (int k = 0; k < bos->bNumDeviceCaps; ++k) {
          struct libusb_bos_dev_capability_descriptor *dev_capability =
              bos->dev_capability[k];
          printf("   BOS Capability %d: Length: %d Descriptor Type %d Dev "
                 "Capability Type: %d\n",
                 k, dev_capability->bLength, dev_capability->bDescriptorType,
                 dev_capability->bDevCapabilityType);
          if (dev_capability->bLength > 3) {
            printf("    Data:");
            for (int l = 0; l < dev_capability->bLength - 3; ++l) {
              printf("  %d", dev_capability->dev_capability_data[l]);
            }
            printf("\n");
          }
        }
        // Explore the device capabilities here
        libusb_free_bos_descriptor(bos);
      }

      libusb_free_config_descriptor(config);
      libusb_close(dev_handle);
    }
  }

  // Free the device list and exit libusb
  libusb_free_device_list(dev_list, 1);
  libusb_exit(ctx);

  return 0;
}
