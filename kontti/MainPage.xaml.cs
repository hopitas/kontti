using System;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;
using kontti.Model;
using Microsoft.Azure.Devices.Client;
using Newtonsoft.Json;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Storage.Streams;
using Windows.Devices.SerialCommunication;
using Windows.Devices.Enumeration;
using System.Linq;

namespace kontti
{

    class ArduinoData
    {
        public double Temperature { get; set; }
        public double Humidity { get; set; }
    }

    public sealed partial class MainPage : Page
    {
        string connectionString;
        private DispatcherTimer readvalTimer;
        private SerialDevice serialPort = null;
        DataReader dataReaderObject = null;
        DataWriter dataWriterObject = null;
        private Data data;
        private byte[] WriteBuf = new byte[1];

        public MainPage()
        {

            this.InitializeComponent();

            data = new Data
            {
                displayname = "KonttiRaspi",
                organization = "T&T",
                timecreated = DateTime.Now.ToLocalTime()
            };

            //Azure connection string, tätä ei sais päästää githubii
            connectionString = "";

            //timer for reading commands every 1sec
            readvalTimer = new DispatcherTimer();
            readvalTimer.Interval = TimeSpan.FromMinutes(10);
            readvalTimer.Tick += readvalTimer_TickAsync;
            readvalTimer.Start();

            ListAvailablePorts();
        }

        //This would read the values from sensors, now generates random number
        private async void readvalTimer_TickAsync(object sender, object e)
        {
            ArduinoData arduinodata = new ArduinoData();

            data.timecreated = DateTime.Now;

                arduinodata = await serialRead();

            Random random = new Random();


            //data.Temperature = random.NextDouble(); //arduinodata.Temperature;
            //data.Humidity = random.NextDouble();//arduinodata.Humidity;

            data.Temperature = Math.Round(arduinodata.Temperature, 2);
            data.Humidity = Math.Round(arduinodata.Humidity, 2);

            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(connectionString, TransportType.Amqp);

            string jsonString = convertData(data);

            sendData(deviceClient, jsonString);

            receiveData(deviceClient);

        }

        private string convertData(Data data)
        {
            string jsonString = JsonConvert.SerializeObject(data);
            Debug.WriteLine(jsonString);
            return jsonString;
        }

        //send data to azure
        static async void sendData(DeviceClient deviceClient, string jsonString)
        {
            var message = new Message(Encoding.ASCII.GetBytes(jsonString));
            await deviceClient.SendEventAsync(message);
        }

        //receive data from azure
        public async void receiveData(DeviceClient deviceClient)
        {
            try
            {
                var receivedMessage = await deviceClient.ReceiveAsync();

                if (receivedMessage != null)
                {
                    var messageData = Encoding.ASCII.GetString(receivedMessage.GetBytes());
                    deviceClient.CompleteAsync(receivedMessage);
                    Debug.WriteLine(messageData);
                }
                else
                {

                }
            }
            catch (Exception e)
            {
                Debug.WriteLine("Exception when receiving message:" + e.Message);
            }
        }

        private async Task<ArduinoData> serialRead()
        {

            dataReaderObject = new DataReader(serialPort.InputStream);
            dataWriterObject = new DataWriter(serialPort.OutputStream);
            ArduinoData data = new ArduinoData();

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
                await dataReaderObject.LoadAsync(256);
                var receivedStrings = dataReaderObject.ReadString(dataReaderObject.UnconsumedBufferLength).Trim();
                data = Newtonsoft.Json.JsonConvert.DeserializeObject<ArduinoData>(receivedStrings);
                Debug.WriteLine(receivedStrings.Trim());
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

            return data;
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
    }
}
