using RGB.NET.Core;

namespace RGB.NET.Devices.MadLed
{
    /// <summary>
    /// Represents a device provider loaded used to dynamically load debug devices into an application.
    /// </summary>
    public class MadLedDeviceProviderLoader : IRGBDeviceProviderLoader
    {
        #region Properties & Fields

        /// <inheritdoc />
        public bool RequiresInitialization => false;

        #endregion

        #region Methods

        /// <inheritdoc />
        public IRGBDeviceProvider GetDeviceProvider() => MadLedDeviceProvider.Instance;

        #endregion
    }
}
