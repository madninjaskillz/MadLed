using System;
using System.Collections.Generic;
using System.Text;

namespace RGB.NET.Devices.MadLed
{
    public class ConfigModel
    {
        public string ComPort { get; set; }
        public List<FanModel> Fans { get; set; }
    }

    public class FanModel
    {
        public int Bank { get; set; }
        public int DataPin { get; set; }
        public string Name { get; set; }
        public int NumberOfLeds { get; set; }
    }
}
