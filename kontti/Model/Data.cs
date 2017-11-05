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

    }

    class ArduinoData
    {
        public double Temperature { get; set; }
        public double Humidity { get; set; }
    }
}
