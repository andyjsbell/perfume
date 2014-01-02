Install and Create project:
    $ npm info cordova
    $ sudo npm install -g cordova
    $ sudo npm update -g cordova
    $ sudo npm install -g cordova@3.1.0
    $ cordova help
    $ cordova info
    $ cordova create hello com.example.hello HelloWorld
    $ cd hello

Run any of these from a Mac:
    $ cordova platform add ios
    $ cordova platform add amazon-fireos
    $ cordova platform add android
    $ cordova platform add blackberry10
    $ cordova platform add firefoxos

Run any of these from a Windows machine
    $ cordova platform add wp7
    $ cordova platform add wp8
    $ cordova platform add windows8
    $ cordova platform add amazon-fireos
    $ cordova platform add android
    $ cordova platform add blackberry10
    $ cordova platform add firefoxos

Platform Management:
    $ cordova platforms ls
    $ cordova platform remove blackberry10
    $ cordova platform rm amazon-fireos
    $ cordova platform rm android
    $ cordova platform update android
    $ cordova platform update ios


Build the App and Test:
    $ cordova build
    $ cordova build ios
    $ cordova prepare ios
    $ cordova compile ios
    $ cordova emulate android
    $ cordova run android

Add Plugin Features:
    $ cordova plugin search bar code
    com.phonegap.plugins.barcodescanner - Scans Barcodes

    # For Basic device information
    $ cordova plugin add org.apache.cordova.device

    # For Network Connection and Battery Events:
    $ cordova plugin add org.apache.cordova.network-information
    $ cordova plugin add org.apache.cordova.battery-status

    # For Accelerometer, Compass, and Geolocation:
    $ cordova plugin add org.apache.cordova.device-motion
    $ cordova plugin add org.apache.cordova.device-orientation
    $ cordova plugin add org.apache.cordova.geolocation

    # For Camera, Media playback and Capture:
    $ cordova plugin add org.apache.cordova.camera
    $ cordova plugin add org.apache.cordova.media-capture
    $ cordova plugin add org.apache.cordova.media

    # For Access files on device or network (File API):
    $ cordova plugin add org.apache.cordova.file
    $ cordova plugin add org.apache.cordova.file-transfer

    # For Notification via dialog box or vibration:
    $ cordova plugin add org.apache.cordova.dialogs
    $ cordova plugin add org.apache.cordova.vibration

    # For Contacts:
    $ cordova plugin add org.apache.cordova.contacts

    # For Globalization:
    $ cordova plugin add org.apache.cordova.globalization

    # For Splashscreen:
    $ cordova plugin add org.apache.cordova.splashscreen

    # For Open new browser windows (InAppBrowser):
    $ cordova plugin add org.apache.cordova.inappbrowser

    # For Debug console:
    $ cordova plugin add org.apache.cordova.console

    ## For plugin management
    $ cordova plugin ls
    $ cordova plugin rm org.apache.cordova.console
    $ cordova plugin remove org.apache.cordova.console
    $ cordova plugin add org.apache.cordova.console org.apache.cordova.device
    $ cordova plugin add org.apache.cordova.console@latest
    $ cordova plugin add org.apache.cordova.console@0.2.1
    $ cordova plugin add https://github.com/apache/cordova-plugin-console.git
    $ cordova plugin add https://github.com/apache/cordova-plugin-console.git#r0.2.0
    $ cordova plugin add https://github.com/someone/aplugin.git#:/my/sub/dir
    $ cordova plugin add https://github.com/someone/aplugin.git#r0.0.1:/my/sub/dir
    $ cordova plugin add ../my_plugin_dir


