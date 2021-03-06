﻿using System;
using System.Collections.Generic;

namespace kontti.Model
{
    class SendData
    {
        public string displayname { get; set; }
        public string organization { get; set; }
        public DateTime timecreated { get; set; }
        public double Temperature { get; set; }
        public double Humidity { get; set; }
        public double Ph { get; set; }
        public bool Lighton { get; set; }
        public bool Watered { get; set; }
        public bool Wleveok { get; set; }
        public DateTime LightSwitchTime { get; set; }
        public DateTime WateredTime { get; set; }
    }

    class MeasurementData
    {
        public double Temperature { get; set; }
        public double Humidity { get; set; }
        public double Ph { get; set; }
        public bool Watered { get; set; }
        public bool Wlevelok { get; set; }
        public bool Lighton { get; set; }
    }

        class Timers
    {
        public DateTime Lightson { get; set; }
        public DateTime Lightsoff { get; set; }
    }
}
