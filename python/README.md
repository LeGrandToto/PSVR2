Simple and straight forward app using pyusb and panel to display usb devices and send/receive data on their endpoints

# INSTALL

## Prerequisits

- python>=3.8
- pip
- virtualenv (Optional but recommended)

## Install

For most linux distros

- Optional (but recommended): create a new virtual environment and activate it:
```bash
virtualenv env
source env/bin/activate
```
- Install the python packages:
```pip install -r requirements.txt

```
*Note:* If this step fails for whatever reason you should instead run `pip install pyusb panel`


## Use the app

```bash
panel serve --autoreload panel.py
```
then head to [localhost:5006/panel](http://localhost:5006/panel). The app will search for all USB devices connected to your PC and for each device,
list the configurations, for each configuration list the interfaces and for each interface list the endpoints. For more details, please read the code.
I would recommend to start reading from the bottom and jump to definitions of what picks your interest.

