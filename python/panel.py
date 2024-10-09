import usb.core
import panel as pn
import param

class USBEndpoint(param.Parameterized):
    usb_endpoint = param.Parameter(None, precedence= -1)

    def ui(self):
        return str(self.usb_endpoint)

class USBInterface(param.Parameterized):
    usb_interface = param.Parameter(None, precedence= -1)

    def ui(self):
        def get_endpoint(usb_endpoint):
            instance = USBEndpoint()
            instance.usb_endpoint = usb_endpoint
            return instance

        endpoints = [get_endpoint(endpoint) for endpoint in self.usb_interface]
        # return str(self.usb_interface)
        return pn.Column(
                pn.Card(
                    str(self.usb_interface),
                    title= "raw",
                    collapsed= True
                    ),
                *[pn.Card(
                    endpoint.ui(),
                    title= f"Endpoint {index}",
                    collapsed= True
                    )
                    for index, endpoint
                    in enumerate(endpoints)
                ]
                )

    def type(self):
        if self.usb_interface.bInterfaceClass == 1:
            return "Audio"
        if self.usb_interface.bInterfaceClass == 2:
            return "Communication"
        if self.usb_interface.bInterfaceClass == 3:
            return "HID"
        if self.usb_interface.bInterfaceClass == 9:
            return "Hub"
        return f"{self.usb_interface.bInterfaceClass=}"

class USBConfiguration(param.Parameterized):
    usb_configuration = param.Parameter(None, precedence= -1)

    def ui(self):
        def get_interface(interface):
            instance = USBInterface()
            instance.usb_interface = interface
            return instance

        interfaces = [get_interface(interface) for interface in self.usb_configuration]
        return pn.Column(
                pn.Card(
                    str(self.usb_configuration),
                    title= "raw",
                    collapsed= True
                ),
                *[pn.Card(
                    interface.ui(),
                    title= f"Interface {index} {interface.type()}",
                    collapsed= True
                    )
                  for index, interface
                  in enumerate(interfaces)
                ],
                )


    def config_name(self):
        return f"{self.usb_configuration.bMaxPower * 2} mA"

class USBDevice(param.Parameterized):
    usb_device = param.Parameter(None, precedence= -1)

    def vendor(self):
        if self.usb_device.idVendor == 0x054c:
            return "SONY"
        return hex(self.usb_device.idVendor)[2:]

    def product(self):
        if self.usb_device.idProduct == 0x0cde:
            return "PlayStation VR2"
        if self.usb_device.idProduct == 0x0e45:
            return "PlayStation VR2 Sense Controller (L)"
        if self.usb_device.idProduct == 0x0e46:
            return "PlayStation VR2 Sense Controller (R)"
        return hex(self.usb_device.idProduct)[2:]

    def ui(self):
        def get_configuration(usb_configuration):
            instance = USBConfiguration()
            instance.usb_configuration = usb_configuration
            return instance
        configurations = [get_configuration(usb_configuration) for usb_configuration in self.usb_device]
        return pn.Card(
                pn.Card(
                    str(self.usb_device),
                    title= "raw",
                    collapsed= True
                ),
                *[pn.Card(
                    configuration.ui(),
                    title= f"{index} Configurations {configuration.config_name()}",
                    collapsed= True
                    ) for index, configuration in enumerate(configurations)],
                title= f"{self.vendor()}:{self.product()}", collapsed= True)

def main(vendor: list[int] = [], product: list[int] = []):
    """
    Args:
        vendor (Optional): The list of vendor IDs to keep
        product (Optional): The list of product IDs to keep
    """
    # Find all USB devices
    devices = usb.core.find(find_all=True)
    if devices is None:
        print(f"No devices found")
        return

    # Only keep USB devices for a specific vendor
    if vendor:
        devices = filter(lambda usb_device: usb_device.idVendor in vendor, devices)

    # Only keep USB devices for a specific product ID
    if product:
        devices = filter(lambda usb_device: usb_device.idProduct in product, devices)

    def get_device(usb_device):
        instance = USBDevice()
        instance.usb_device = usb_device
        return instance

    devices = sorted([get_device(usb_device) for usb_device in devices], key= lambda obj: obj.vendor(), reverse= True)
    # Instantiate the template with widgets displayed in the sidebar
    template = pn.template.FastListTemplate(
        title="USB Prober",
        main = [device.ui() for device in devices],
    )
    template.servable()

main()
