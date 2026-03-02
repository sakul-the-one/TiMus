from ast import While
import sys
import glob
import serial
import time

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/ttyACM*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

ser = []
message = []  

def UpdateSerial():
    serials = serial_ports()
    serN = []
    i = 0
    while i < len(serials):
        #ser.append(serial.Serial(serials[i], timeout=0.25))
        if IsInSerials(serials[i]):#serials[i].name in ser:
             0+0
        else:
            temp = serial.Serial(serials[i], timeout=0.25)
            temp.write(b"srv: Connection succesfull\0")
            #message.append("srv: new Connection made")
            #time.sleep(1)
            #temp.write(b"srv: Test message\0\0\0\0\0\0\0\0\0\0\0\0\0\0srv: Tes message #2\0")
            ser.append(temp)
            print("Connected to: " + temp.name)
        i += 1

def IsInSerials(name):
    return any(s.name == name for s in ser)
#    i = 0
#    while i < len(ser):
#        if ser[i].name == name:
#            return True
#    print("Stuck63")
#    i += 1
#    return False

if __name__ == '__main__':
    #print(serial_ports())
    UpdateSerial()
    print("Init done")
    message = []
    while message != "777: !STOP\0":
        #Read all messages
        error = -1
        message = []
        i = 0
        while i < len(ser):
            #print("trying to read: " + ser[i].name)
            try:
                temp = ser[i].read(32)
                if(temp != b""):
                    print(temp)
                    message.append(temp)
            except:
                error = i
            i += 1
        #Distrubita all Messages
        if len(message) != 0:
            i = 0
            while i < len(ser):
                try:
                    j = 0
                    while j < len(message):
                        ser[i].write(message[j])
                        j += 1      
                except:
                    error = i # We can only remove one at a time :)
                i += 1
        #remove 1 element
        if error != -1:
            #print("Disconned: " + error)
            #print("Ser.lenght: " + len(ser))
            print("Disconnedted: " + ser[error].name)
            ser[error].close()
            ser.pop(error)
        #Check for new members
        i = 0
        #message = []
        UpdateSerial()