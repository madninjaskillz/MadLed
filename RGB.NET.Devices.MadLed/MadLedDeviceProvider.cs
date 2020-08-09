// ReSharper disable MemberCanBePrivate.Global
// ReSharper disable UnusedMember.Global

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.Serialization.Json;
using System.Threading.Tasks;
using RGB.NET.Core;

namespace RGB.NET.Devices.MadLed
{
    /// <inheritdoc />
    /// <summary>
    /// Represents a device provider responsible for debug devices.
    /// </summary>
    public class MadLedDeviceProvider : IRGBDeviceProvider
    {
        #region Properties & Fields

        private static MadLedDeviceProvider _instance;
        /// <summary>
        /// Gets the singleton <see cref="MadLedDeviceProvider"/> instance.
        /// </summary>
        public static MadLedDeviceProvider Instance => _instance ?? new MadLedDeviceProvider();
        public DeviceUpdateTrigger UpdateTrigger { get; }
        /// <inheritdoc />
        public bool IsInitialized { get; private set; }

        public MadLedSerialDriver MadLedDriver;

        /// <inheritdoc />
        public IEnumerable<IRGBDevice> Devices { get; private set; }

        /// <inheritdoc />
        public bool HasExclusiveAccess { get; private set; }

        private List<(string layout, string imageLayout, Func<Dictionary<LedId, Color>> syncBackFunc, Action<IEnumerable<Led>> updateLedsAction)> _fakeDeviceDefinitions
            = new List<(string layout, string imageLayout, Func<Dictionary<LedId, Color>> syncBackFunc, Action<IEnumerable<Led>> updateLedsAction)>();

        #endregion

        #region Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="MadLedDeviceProvider"/> class.
        /// </summary>
        /// <exception cref="InvalidOperationException">Thrown if this constructor is called even if there is already an instance of this class.</exception>
        public MadLedDeviceProvider()
        {
            if (_instance != null) throw new InvalidOperationException($"There can be only one instance of type {nameof(MadLedDeviceProvider)}");
            _instance = this;
            UpdateTrigger = new DeviceUpdateTrigger();
            //Should i stick the json loading here?
        }

        #endregion

        #region Methods

        public ConfigModel GetConfig()
        {
            //todo read from file
            return new ConfigModel
                   {
                       ComPort = "COM3",
                       Fans = new List<FanModel>
                              {
                                  new FanModel{ Bank=0, DataPin = 3, Name="Bottom Front", NumberOfLeds = 21},
                                  new FanModel{ Bank=1, DataPin = 4, Name="Top Front", NumberOfLeds = 21},
                              }
                   };
        }

        /// <inheritdoc />
        public bool Initialize(RGBDeviceType loadFilter = RGBDeviceType.Unknown, bool exclusiveAccessIfPossible = false, bool throwExceptions = false)
        {
            IsInitialized = false;
            try
            {
                UpdateTrigger?.Stop();
                HasExclusiveAccess = exclusiveAccessIfPossible;

                List<IRGBDevice> devices = new List<IRGBDevice>();
                ConfigModel config = GetConfig();
                MadLedDriver = new MadLedSerialDriver(config.ComPort);
                
                foreach (FanModel fan in config.Fans)
                {
                    MadLedDeviceUpdateQueue queue = new MadLedDeviceUpdateQueue(UpdateTrigger,"MadLed", MadLedDriver,fan);
                    MadLedDriver.AddDevice(fan.Name, fan.Bank, fan.DataPin, fan.NumberOfLeds);

                    for (int i = 0; i < fan.NumberOfLeds; i++)
                    {
                        MadLedDriver.SetLED(fan.Bank,i, 0,0,0);
                    }

                    MadLedDriver.Present();

                    for (int i = 0; i < fan.NumberOfLeds; i++)
                    {
                        MadLedDriver.SetLED(fan.Bank, i, 0, 0, 255);
                        MadLedDriver.Present();
                        Task.Delay(20).Wait();
                    }

                    for (int i = 0; i < fan.NumberOfLeds; i++)
                    {
                        MadLedDriver.SetLED(fan.Bank, i, 0, 0, 0);
                        MadLedDriver.Present();
                        Task.Delay(20).Wait();
                    }

                    MadLedRGBDevice device = new MadLedRGBDevice(fan,MadLedDriver, null, null );
                    device.Initialize(queue);
                    devices.Add(device);
                }

                Devices = new ReadOnlyCollection<IRGBDevice>(devices);
                IsInitialized = true;

                MadLedDriver.ListDevices();
                UpdateTrigger?.Start();
                return true;
            }
            catch
            {
                if (throwExceptions) throw;
                return false;
            }

            
        }

        /// <inheritdoc />
        public void ResetDevices()
        { }

        /// <inheritdoc />
        public void Dispose()
        {
            _fakeDeviceDefinitions.Clear();
        }

        #endregion
    }
}
