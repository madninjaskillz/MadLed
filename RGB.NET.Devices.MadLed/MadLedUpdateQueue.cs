using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using RGB.NET.Core;

namespace RGB.NET.Devices.MadLed
{

    /// <summary>
    /// Represents the update-queue performing updates for MSI devices.
    /// </summary>
    public class MadLedDeviceUpdateQueue : UpdateQueue
    {
        #region Properties & Fields

        private MadLedSerialDriver driver;
        private string _deviceType;
        private FanModel fanModel;

        #endregion

        #region Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="MsiDeviceUpdateQueue"/> class.
        /// </summary>
        /// <param name="updateTrigger">The update trigger used by this queue.</param>
        /// <param name="deviceType">The device-type used to identify the device.</param>
        public MadLedDeviceUpdateQueue(IDeviceUpdateTrigger updateTrigger, string deviceType, MadLedSerialDriver drv,
            FanModel fanModel)
            : base(updateTrigger)
        {
            this._deviceType = deviceType;
            this.driver = drv;
            this.fanModel = fanModel;
        }

        #endregion

        #region Methods

        /// <inheritdoc />
        private bool toggle = true;

        private bool ignoreCache = false;
        protected override void Update(Dictionary<object, Color> dataSet)
        {
            List<Tuple<int,int?>>batches=new List<Tuple<int, int?>>();
            IEnumerable<IGrouping<Color, KeyValuePair<object, Color>>> dataSetGroup = dataSet.GroupBy(x => x.Value);
            foreach (var group in dataSetGroup)
            {
                
                int r = group.Key.GetR();
                int g = group.Key.GetG();
                int b = group.Key.GetB();

                List<LedId> leds = group.Select(p => (LedId) p.Key).ToList();
                List<int> ledInts = leds.Select(x => ((int) x) - ((int) LedId.Fan1)).ToList();

                var lowestAsEnum = (LedId)group.Min(x => ((int) x.Key));
                var highestAsEnum = (LedId) group.Max(x => ((int) x.Key));

                int lowest = group.Min(x => ((int)x.Key) - 0x00B00001);
                int highest = group.Max(x => ((int)x.Key) - 0x00B00001);
                int groupStart = lowest;
                int ptr = lowest;

                int start = lowest;
                int end = 0;
                for (int i = lowest; i <= highest+1; i++)
                {
                    if (ledInts.Contains(i))
                    {
                        if (start > -1)
                        {
                            end = i;
                        }
                        else
                        {
                            start = i;
                            end = 0;
                        }
                    }
                    else
                    {
                        if (start > -1)
                        {
                            end = i - 1;
                            if (end - start > 1)
                            {
                                batches.Add(new Tuple<int, int?>(start,end));
                            }
                            else
                            {
                                batches.Add(new Tuple<int, int?>(start,null));
                            }

                            start = -1;
                            end = 0;
                        }
                    }
                }

                foreach (var batch in batches)
                {
                    if (batch.Item2!=null)
                    {
                        driver.BatchSetLED(fanModel.Bank, batch.Item1,batch.Item2.Value, r, g, b);
                    }
                    else
                    {
                        driver.SetLED(fanModel.Bank,batch.Item1, r,g,b );
                    }
                }


            }
            

            driver.Present();

        }


        private int ToInt(float cl) => (int) (255 * cl);
        Dictionary<int, Tuple<int, int, int>> currentSets = new Dictionary<int, Tuple<int, int, int>>();
    }

    #endregion


    }
    
