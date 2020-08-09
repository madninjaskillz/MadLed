using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using RGB.NET.Core;
using RGB.NET.Core.Layout;

namespace RGB.NET.Devices.MadLed
{
    /// <inheritdoc />
    /// <summary>
    /// Represents a debug device.
    /// </summary>
    public class MadLedRGBDevice : AbstractRGBDevice<MadLedRGBDeviceInfo>
    {
        #region Properties & Fields

        /// <inheritdoc />
         public int Bank { get; set; }
        public override MadLedRGBDeviceInfo DeviceInfo { get; }
        private MadLedSerialDriver driver;
        protected MadLedDeviceUpdateQueue DeviceUpdateQueue { get; set; }
        private Func<Dictionary<LedId, Color>> _syncBackFunc;
        private Action<IEnumerable<Led>> _updateLedsAction;
        private bool toggle = true;
        private FanModel fanModel;
        #endregion

        #region Constructors
        /// <summary>
        /// Internal constructor of <see cref="MadLedRGBDeviceInfo"/>.
        /// </summary>
        internal MadLedRGBDevice(FanModel fanModel, MadLedSerialDriver drv, Func<Dictionary<LedId, Color>> syncBackFunc = null, Action<IEnumerable<Led>> updateLedsAction = null)
        {
            this.driver = drv;
            this._syncBackFunc = syncBackFunc;
            this._updateLedsAction = updateLedsAction;
            this.fanModel = fanModel;

            //DeviceLayout layout = DeviceLayout.Load(layoutPath);

            DeviceInfo = new MadLedRGBDeviceInfo(RGBDeviceType.Fan, "Mad Ninja","MadLed", RGBDeviceLighting.Key, syncBackFunc != null, fanModel.Name);

            for (int i = 0; i < fanModel.NumberOfLeds; i++)
            {
                LedId ledid = ((LedId)0x00B00001 + i);
                //Led led = new Led(this, ledid, new Point(1,1), new Size(1,1), i );
                //LedMapping.Add(ledid, led);

                InitializeLed(ledid, new Point(1, 1), new Size(1, 1));
            }

            Debug.WriteLine("Setup new fan on bank "+fanModel.Bank);
        }

        #endregion

        #region Methods

        internal void Initialize(string layoutPath, string imageLayout) => ApplyLayoutFromFile(layoutPath, imageLayout, true);

        public void Initialize(MadLedDeviceUpdateQueue updateQueue)
        {
            DeviceUpdateQueue = updateQueue;

            //InitializeLayout(ledCount);

            if (Size == Size.Invalid)
            {
                Rectangle ledRectangle = new Rectangle(this.Select(x => x.LedRectangle));
                Size = ledRectangle.Size + new Size(ledRectangle.Location.X, ledRectangle.Location.Y);
            }
        }

        /// <inheritdoc />
        public override void SyncBack()
        {
            try
            {
                Dictionary<LedId, Color> syncBackValues = _syncBackFunc?.Invoke();
                if (syncBackValues == null) return;

                foreach (KeyValuePair<LedId, Color> value in syncBackValues)
                {
                    Led led = ((IRGBDevice)this)[value.Key];
                    SetLedColorWithoutRequest(led, value.Value);
                }
            }
            catch {/* idc that's not my fault ... */}
        }

        /// <inheritdoc />
        protected override void UpdateLeds(IEnumerable<Led> ledsToUpdate)
        {
            
            var uleds = ledsToUpdate.Where(x => (x.Color.A > 0));
            DeviceUpdateQueue.SetData(uleds);
        }

        
        #endregion
    }
}
