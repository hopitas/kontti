﻿using System;
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

    public sealed partial class MainPage : Page
    {
        string connectionString;
        private DispatcherTimer sendToCloudTimer;
        private DispatcherTimer arduinoTimer;
        private SerialDevice serialPort = null;
        DataReader dataReaderObject = null;
        DataWriter dataWriterObject = null;
        private SendData envdata;
        private Timers timers;
        private byte[] WriteBuf = new byte[1];
        DateTime wt1, wt2, wt3, wt4;
        bool Wateredtemp = false;
        bool lightonTemp = false;
        bool lightoffTemp = false;

        public MainPage()
        {

            this.InitializeComponent();

            envdata = new SendData
            {
                displayname = "KonttiRaspi",
                organization = "T&T",
                timecreated = DateTime.Now.ToLocalTime(),
                Watered = false,
                Lighton = true
            };

            wt1 = new DateTime(2017, 12, 3, 6, 0, 0);
            wt2 = new DateTime(2017, 12, 3, 12, 0, 0);
            wt3 = new DateTime(2017, 12, 3, 18, 0, 0); //18
            wt4 = new DateTime(2017, 12, 3, 0, 00, 0);

            //Default timers
            timers = new Timers
            {
                Lightson = new DateTime(2017, 12, 3, 7, 0, 0),
                Lightsoff = new DateTime(2017, 12, 3, 23, 0, 0)
            };

            //Azure connection string, tätä ei sais päästää githubii
            connectionString = "";

            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(connectionString, TransportType.Amqp);

            //Start waiting for Azure data
            ReceiveC2dAsync(deviceClient);

            // Find arduino com port
            ListAvailablePorts();

            //timer for sending sensor data to cloud every 10 minutes
            sendToCloudTimer = new DispatcherTimer();
            sendToCloudTimer.Interval = TimeSpan.FromMinutes(10);
            sendToCloudTimer.Tick += sendToCloudTimer_TickAsync;
            sendToCloudTimer.Start();

            //timer for communicating with Arduino every 20 seconds
            arduinoTimer = new DispatcherTimer();
            arduinoTimer.Interval = TimeSpan.FromSeconds(20);
            arduinoTimer.Tick += arduinoTimer_TickAsync;
            arduinoTimer.Start();
        }

        //Sends values to cloud
        private async void sendToCloudTimer_TickAsync(object sender, object e)
        {
            sendAzure();
        }

        //Communicates with Arduino
        private async void arduinoTimer_TickAsync(object sender, object e)
        {
            MeasurementData arduinodata = new MeasurementData();

            //watering
            if ((wt1.TimeOfDay.Minutes.Equals(DateTime.Now.TimeOfDay.Minutes) && wt1.TimeOfDay.Hours.Equals(DateTime.Now.TimeOfDay.Hours) && Wateredtemp == false)
                ||
                (wt2.TimeOfDay.Minutes.Equals(DateTime.Now.TimeOfDay.Minutes) && wt2.TimeOfDay.Hours.Equals(DateTime.Now.TimeOfDay.Hours) && Wateredtemp == false)
                ||
                (wt3.TimeOfDay.Minutes.Equals(DateTime.Now.TimeOfDay.Minutes) && wt3.TimeOfDay.Hours.Equals(DateTime.Now.TimeOfDay.Hours) && Wateredtemp == false)
                ||
                (wt4.TimeOfDay.Minutes.Equals(DateTime.Now.TimeOfDay.Minutes) && wt4.TimeOfDay.Hours.Equals(DateTime.Now.TimeOfDay.Hours) && Wateredtemp == false)
                )
            {
                try
                {
                    arduinodata = await serialRead(2);
                    Debug.WriteLine("Watering");
                    Wateredtemp = true;
                }
                catch (Exception ex)
                {
                    Debug.WriteLine(ex.Message);
                    Debug.WriteLine("Could not send data to Arduino");
                    ListAvailablePorts();
                }
            }
            // lightson timer
            else if ((timers.Lightson.TimeOfDay.Minutes.Equals(DateTime.Now.TimeOfDay.Minutes) && timers.Lightson.TimeOfDay.Hours.Equals(DateTime.Now.TimeOfDay.Hours)) && lightonTemp == false)
            {
                try
                {
                    arduinodata = await serialRead(3); //change
                    Debug.WriteLine("Lights on!");
                    lightonTemp = true;
                }
                catch (Exception ex)
                {
                    Debug.WriteLine(ex.Message);
                    Debug.WriteLine("Could not send data Arduino");
                    ListAvailablePorts();
                }
            }
            //lights off timer
            else if ((timers.Lightsoff.TimeOfDay.Minutes.Equals(DateTime.Now.TimeOfDay.Minutes) && timers.Lightsoff.TimeOfDay.Hours.Equals(DateTime.Now.TimeOfDay.Hours)) && lightoffTemp == false)
            {
                try
                {
                    arduinodata = await serialRead(4);
                    Debug.WriteLine("Lights off!");
                    lightoffTemp = true;
                }
                catch (Exception ex)
                {
                    Debug.WriteLine(ex.Message);
                    Debug.WriteLine("Could not send data Arduino");
                    ListAvailablePorts();
                }
            }
            else
            {
                try
                {
                    arduinodata = await serialRead(1);
                }
                catch (Exception ex)
                {
                    Debug.WriteLine(ex.Message);
                    Debug.WriteLine("Could not get data from Arduino");
                    ListAvailablePorts();
                }
            }

            if (arduinodata != null && arduinodata.Temperature != 0 && arduinodata.Humidity != 0)
            {
                envdata.Temperature += Math.Round(arduinodata.Temperature, 2);
                envdata.Humidity += Math.Round(arduinodata.Humidity, 2);
                envdata.Ph += Math.Round(arduinodata.Ph, 2);
                envdata.Temperature = envdata.Temperature / 2;
                envdata.Humidity = envdata.Humidity / 2;
                envdata.Ph = envdata.Ph / 2;
            }
            if (lightonTemp == true)
            {
                envdata.Lighton = true;
                envdata.LightSwitchTime = DateTime.Now;
                lightonTemp = false;
            }
            if (lightoffTemp == true)
            {
                envdata.Lighton = false;
                envdata.LightSwitchTime = DateTime.Now;
                lightoffTemp = false;
            }
            if (Wateredtemp == true)
            {
                envdata.WateredTime = DateTime.Now;
                envdata.Watered = true; //Sends azure if watered in this 10min cycle                      
                envdata.Wleveok = arduinodata.Wlevelok;
              //  arduinodata.Watered = false;
                Wateredtemp = false;
            }
        }

        private async void sendAzure()
        {
            //Random random = new Random();
            //data.Temperature = random.NextDouble(); //arduinodata.Temperature;
            //data.Humidity = random.NextDouble();//arduinodata.Humidity;

            envdata.timecreated = DateTime.Now;

            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(connectionString, TransportType.Amqp);

            string jsonString = convertData(envdata);

            sendData(deviceClient, jsonString);

            //set the default values
            envdata.Watered = false;
        }

        private string convertData(SendData data)
        {
            string jsonString = "";
            try
            {
                jsonString = JsonConvert.SerializeObject(data);
                Debug.WriteLine(jsonString);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                Debug.WriteLine("Json parse error");
            }

            return jsonString;
        }

        //send data to azure
        static async void sendData(DeviceClient deviceClient, string jsonString)
        {
            var message = new Message(Encoding.ASCII.GetBytes(jsonString));
            await deviceClient.SendEventAsync(message);
        }

        //Receive from Azure
        private static async void ReceiveC2dAsync(DeviceClient deviceClient)
        {
            while (true)
            {
                Message receivedMessage = await deviceClient.ReceiveAsync();
                if (receivedMessage == null) continue;
                await deviceClient.CompleteAsync(receivedMessage);
                Debug.WriteLine(receivedMessage);
            }
        }

        private async Task<MeasurementData> serialRead(byte caseArduino)
        {

            dataReaderObject = new DataReader(serialPort.InputStream);
            dataWriterObject = new DataWriter(serialPort.OutputStream);
            MeasurementData data = new MeasurementData();

            try
            {
                Task<UInt32> storeAsyncTask1;
                WriteBuf[0] = caseArduino;
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
                    data = JsonConvert.DeserializeObject<MeasurementData>(receivedStrings);
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
                Debug.WriteLine("Arduino not connected! \n" + ex.Message);
                //  serialPort.Dispose();  
            }
        }
    }
}
