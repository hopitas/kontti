using System;
using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using Windows.Devices.SerialCommunication;
using Windows.Storage.Streams;
using System.Threading.Tasks;
using System.Diagnostics;

namespace SerialSample
{
    public sealed partial class MainPage : Page
    {

        private SerialDevice serialPort = null;

        DataReader dataReaderObject = null;
        DataWriter dataWriterObject = null;
        private DispatcherTimer readvalTimer;
        private byte[] WriteBuf = new byte[1];

        public uint BytesReceived { get; }

        public MainPage()
        {
            InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs navArgs)
        {
            ListAvailablePorts();

            readvalTimer = new DispatcherTimer();
            readvalTimer.Interval = TimeSpan.FromSeconds(10);
            readvalTimer.Tick += readvalTimer_Tick;
            readvalTimer.Start();
        }

        public void readvalTimer_Tick(object sender, object e)
        {
            serialRead();
        }

        private async void ListAvailablePorts()
        {
            try
            {
                string aqs = SerialDevice.GetDeviceSelector();
                var dis = await DeviceInformation.FindAllAsync(aqs);
                var selectedPort = dis.Where(t => t.Name == "Genuino Uno").First();

                serialPort = await SerialDevice.FromIdAsync(selectedPort.Id);


                serialPort.ReadTimeout = TimeSpan.FromMilliseconds(1000);
                serialPort.BaudRate = 115200;
                serialPort.Parity = SerialParity.None;
                serialPort.StopBits = SerialStopBitCount.One;
                serialPort.DataBits = 8;

            }

            catch (Exception ex)
            {
                Debug.WriteLine("OOps, Something went wrong! \n" + ex.Message);
            }
        }


        private async void serialRead()
        {
            dataReaderObject = new DataReader(serialPort.InputStream);
            dataWriterObject = new DataWriter(serialPort.OutputStream);

            try
            {
                Task<UInt32> storeAsyncTask1;

                WriteBuf[0] = 1;
                dataWriterObject.WriteBytes(WriteBuf);

                storeAsyncTask1 = dataWriterObject.StoreAsync().AsTask();

                UInt32 bytesWritten1 = await storeAsyncTask1;

                // Cleanup once complete
                if (dataWriterObject != null)
                {
                    dataWriterObject.DetachStream();
                    dataWriterObject = null;
                }
                else
                {
                    Debug.WriteLine("Could not write to Arduino");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                Debug.WriteLine("Could not write to Arduino");
            }

            try
            {
                var bytesRecieved = await dataReaderObject.LoadAsync(15);

                if (bytesRecieved > 0)
                {

                    //    data.Text = dataReaderObject.ReadString(bytesRecieved).Trim();
                    Debug.WriteLine(dataReaderObject.ReadString(bytesRecieved).Trim());
                }

            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                ListAvailablePorts();
            }
            finally
            {
                if (dataReaderObject != null)
                {
                    dataReaderObject.DetachStream();
                    dataReaderObject = null;
                }
            }
        }
    }
}
