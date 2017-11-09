using System;

namespace kontti.Model
{
    class Data
    {
        public string displayname { get; set; }
        public string organization { get; set; }
        public DateTime timecreated { get; set; }
        public double Temperature { get; set; }
        public double Humidity { get; set; }
        public bool Lighton { get; set; }
        public bool Watered { get; set; }
        public DateTime LightSwitchTime { get; set; }
        public DateTime WateredTime { get; set; }
    }

    class ArduinoData
    {
        public double Temperature { get; set; }
        public double Humidity { get; set; }
        public bool Watered { get; set; }
        public bool Lighton { get; set; }
    }
}
