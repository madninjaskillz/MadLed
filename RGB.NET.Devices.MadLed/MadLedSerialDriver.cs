using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Text;
using System.Threading.Tasks;


namespace RGB.NET.Devices.MadLed
{
    public class MadLedSerialDriver : IDisposable
    {
        private System.IO.Ports.SerialPort myPort;
        private Action<string> getNextLine;
        public string SerialPort { get; set; }
        private string sendBuffer = "";
        public MadLedSerialDriver(string comPort)
        {
            SerialPort = comPort;

            myPort = new SerialPort(SerialPort, 19200);
            myPort.Open();
            myPort.DataReceived += MyPort_DataReceived;
            SendCommandInstant("flush");
            Debug.WriteLine(Ping().TotalMilliseconds+"ms");
        }

        private string dataReceived = "";
        private void MyPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string indataall = sp.ReadExisting().Replace("\r", "");

            dataReceived = dataReceived + indataall;

            string tmp = dataReceived;
            while (tmp.Contains("\n"))
            {
                var splitee = tmp.Split('\n');
                if (splitee.Length > 1)
                {
                    string firstLine = splitee[0];
                    tmp = tmp.Substring(firstLine.Length + 1);
                    if (!firstLine.StartsWith(">"))
                    {
                        Debug.WriteLine("}"+firstLine);
                        getNextLine?.Invoke(firstLine);
                        getNextLine = null;
                    }
                    else
                    {
                        Debug.WriteLine(firstLine);
                    }
                }
            }

            dataReceived = tmp;
        }

        DateTime lastSend=DateTime.MinValue;
        public void Write(string cmd)
        {
            if ((DateTime.Now - lastSend).TotalMilliseconds > 10)
            {
                Task.Delay(10).Wait();
            }

            myPort.Write(cmd);
        }

        public void SendCommand(string cmd)
        {
            //if (sendBuffer.Length > 0)
            //{
            //    sendBuffer = sendBuffer + "~";
            //}

            sendBuffer = sendBuffer + cmd+"~";

        }

        public void Present()
        {
            if (sendBuffer.Length > 0)
            {
                Write(sendBuffer);
                sendBuffer = "";
            }
        }

        public void SendCommandInstant(string cmd)
        {
            Write(cmd+"~");
        }

        public void AddDevice(string name, int bank, int pin, int leds)
        {
            //add:0:3:21:Bottom Front

            string cmd = $"add:{bank}:{pin}:{leds}:{name}";
            Debug.WriteLine(">>>" + cmd);
            SendCommandInstant(cmd);
            
        }

        public void SetLED(int bank, int led, int r, int g, int b)
        {
            //set:0:15:0:255:0
            string cmd = $"set:{bank}:{led}:{r}:{g}:{b}";
            SendCommand(cmd);
            // Present();
        }

        public void BatchSetLED(int bank, int led,int led2, int r, int g, int b)
        {
            //set:0:15:0:255:0
            string cmd = $"bulk:{bank}:{led}-{led2}:{r}:{g}:{b}";
            SendCommand(cmd);
            // Present();
        }

        public TimeSpan Ping()
        {
            DateTime start=DateTime.Now;

            bool gotResponse = false;

            getNextLine = s =>
            {
                if (s.Contains("pong"))
                {
                    gotResponse = true;
                    getNextLine = null;
                }
            };

                SendCommandInstant("ping");

            while (!gotResponse)
            {
                if ((DateTime.Now - start).TotalSeconds > 10)
                {
                    throw new TimeoutException("Took more than 10 seconds");
                }
            }

            return (DateTime.Now - start);
        }

        public void ListDevices()
        {
            string cmd = "devices";
            SendCommandInstant(cmd);
            
        }

        public Tuple<int, int, int> GetLED(int bank, int led)
        {
            int r = 0;
            int g = 0;
            int b = 0;
            string cmd = $"get:{bank}:{led}";
            bool gotResponse = false;

            getNextLine = s =>
            {
                if (!s.StartsWith(">"))
                {
                    var parts = s.Split(':');
                    r = int.Parse(parts[0]);
                    g = int.Parse(parts[1]);
                    b = int.Parse(parts[2]);
                    getNextLine = null;
                    gotResponse = true;
                }
            };

            SendCommandInstant(cmd);
            
            while (!gotResponse)
            {
                Task.Delay(1).Wait();
            }

            return new Tuple<int, int, int>(r, g, b);
        }


        public void Dispose()
        {
            myPort?.Close();
            myPort?.Dispose();
        }
    }
}
