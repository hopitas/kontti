using System;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;
using kontti.Model;
using Microsoft.Azure.Devices.Client;
using Newtonsoft.Json;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace kontti
{
    public sealed partial class MainPage : Page
    {
        string connectionString;
        private DispatcherTimer readvalTimer;
        private Data data;

        public MainPage()
        {
            this.InitializeComponent();

            data = new Data
            {
                displayname = "KonttiRaspi",
                organization = "T&T",
                timecreated = DateTime.Now.ToLocalTime(),
                randomnumber = 123.23
            };

            //Azure connection string
            connectionString = "HostName=tthub.azure-devices.net;DeviceId=kontti;SharedAccessKey=s4S7sDjCRIiPf03gX2axgof8ZSOz2qgL9OeEmI7aS8o=";

            //timer for reading commands every 1sec
            readvalTimer = new DispatcherTimer();
            readvalTimer.Interval = TimeSpan.FromSeconds(1);
            readvalTimer.Tick += readvalTimer_Tick;
            readvalTimer.Start();
        }

        //This would read the values from sensors, now generates random number
        private void readvalTimer_Tick(object sender, object e)
        {
            Random rand = new Random();
            data.timecreated = DateTime.Now.ToLocalTime();
            data.randomnumber = rand.NextDouble();
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

    }
}
