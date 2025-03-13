## Full Design of Arduino System
The following outlines the full design of the EMG-Data Acquisition System

### Components
- MyoWare EMG Sensor
- Arduino R3
- DC Motors
- Current Sensors
- 5V Battery

```mermaid
graph TD
  subgraph Full Design
    A[MyoWare EMG Sensor]
    
    subgraph Arduino
      C1[Input Pins]
      C2[ADC]
      C3[Digital Filter]
      C4[Control Algorithm]
      C5[Output Pins]
    end

    subgraph Hand
        B1[DC Motors]
        B2[Current Sensor]
    end
    
    A -- "EMG Data" --> C1 --> C2 -- "Digital EMG Signal" --> C3 -- "Filtered EMG Data" --> C4 --> C5 -- "Digital Control Signal" --> B1 -- "DC Current" --> B2 -- "Current Reading" --> C1
  end
```