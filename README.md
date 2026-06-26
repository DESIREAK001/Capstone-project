# SmartBeam: An Embedded Program for Automatic Headlight Intensity Adjustment

SmartBeam is an affordable automotive retrofit solution designed to automatically modulate vehicle headlight intensity in real time. By dynamically switching between high beam, low beam, and intermediate states based on oncoming traffic and ambient lighting, the system mitigates the dangerous effects of headlight glare, reduces glare-induced driver fatigue, and enhances night-time road safety in resource-constrained environments.

This system was designed, built, and validated as part of a capstone project for the partial fulfillment of the award of a Bachelor's Degree of Science in Computer Science at Gulu University. All technical parameters, circuit constraints, and system designs referenced here are derived from the project report file named **"SMART_BEAM1 (3).pdf"**.

---

## 📌 Table of Contents
* [Project Background & Problem Statement](#-project-background--problem-statement)
* [System Objectives](#-system-objectives)
* [System Architecture & Logical Design](#-system-architecture--logical-design)
* [Hardware Specification & Wiring Layout](#-hardware-specification--wiring-layout)
* [Software Implementation & Calibration Thresholds](#-software-implementation--calibration-thresholds)
* [Validation Protocols & Test Results](#-validation-protocols--test-results)
* [Future Research & Enhancements](#-future-research--enhancements)
* [Project Contribution & Credits](#-project-contribution--credits)

---

## 🔍 Project Background & Problem Statement

Night-time driving is a necessity for long-distance transport, yet visibility remains heavily compromised by irresponsible driving habits and inadequate technological adoption. Approximately half of all traffic deaths occur during night-time hours (between 8:00 p.m. and 11:59 p.m.). A primary catalyst for these accidents is temporary blindness and reduced reaction times caused by the blinding glare of high beams from oncoming vehicles.

While high-end modern vehicles feature factory-installed adaptive high-beam assist or matrix LED frameworks, these vehicles likely constitute less than 5% of the active fleet on Ugandan roads. The remaining 95% consists of older passenger sedans and SUVs that lack automated glare-control architectures. SmartBeam bridges this safety gap by delivering an affordable, context-specific retrofit package engineered using accessible embedded hardware and a closed-loop control system.

---

## 🎯 System Objectives

* **Investigate Limitations:** Evaluate the gaps and strengths of baseline automatic lights, adaptive headlights, high-beam assist, and matrix illumination systems.
* **Design an Embedded Program:** Develop logical, low-latency code mapping physical light data to deterministic hardware states using advanced sensor technology.
* **Implement Prototype:** Assemble a bench-top, closed-loop functional prototype capable of simulating automotive beam transformations safely.
* **Validate Performance:** Test response times, environmental resilience, and loop stability within a controlled laboratory environment.

---

## 🏗️ System Architecture & Logical Design

The system relies on a **closed-loop feedback control architecture** designed to actively intercept and correct real-time hardware desynchronization.

### Data Flow Diagram (DFD) Level 0 Overview
```
                      ┌────────────────────────┐
                      │  Ambient Environment   │
                      │    (Outside Light)     │
                      └───────────┬────────────┘
                                  │
                            Ambient Sensors
                                  │
                                  ▼

```

The system transitions through operational data stages as detailed in the source project configurations:
* **Sensing Transformation:** Process 1.0 measures ambient lux via a front-facing sensor array, while Process 2.0 reads continuous luminosity directly from an isolated sub-section of the vehicle's headlight lens, digitizing both parameters into 10-bit values (0 to 1023).
* **Serialization & Buffer:** Readings are cached in temporary registers (Data Store D1 for Ambient, Data Store D2 for Feedback) to decouple hardware speeds from software execution.
* **Central Processing Engine:** Process 3.0 parses the data, routing output vectors concurrently to a hardware evaluation matrix and system diagnostics via terminal serialization.
* **Target State Determination:** Process 4.0 cross-references ambient values against limits stored in Data Store D3 (`State Configuration`). Process 5.0 checks this computed target against the active profile in Data Store D4 (`Current Verified State`). If states match, execution ends.
* **Actuation Correction:** Upon detecting a divergence, Process 6.0 sends a precise, 150ms Active-LOW pulse out to the physical Relay Module. This forces a sequence shift, which is instantly captured and verified by the local feedback loop, locking down stability.

---

## 🔌 Hardware Specification & Wiring Layout

The bench-top prototype was developed using durable, high-availability electrical components to simulate an automotive power network.

### Component Wiring Matrix
To protect processing logic from vehicle voltage disruptions, wiring paths were executed based on the following structural pin maps:

| Source Component | Pin Out Reference | Target Component | Pin In Reference | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| **Buck Regulator** | Vout AWG Solid Jumper | **Arduino Uno** | 5V Pin22 | Steps vehicle source power down to safe logic tolerances |
| **LDR Sensor Array** | Signal Out | **Arduino Uno** | Pin A0 | Continuously logs environmental lux levels |
| **Arduino Uno** | Pin 7 | **Relay Module** | Signal/In | Drives pulse instructions to the physical switch loop  |
| **Relay Output (COM)** | Common | **Battery Vehicle (12V)**| positive (+) | Channels the main illumination system source power  |
| **Relay Output (NC)** | Normally Closed | **Headlamp Module**| High-Beam Pin | Maps to high beams by default to prioritize safety  |
| **Relay Output (NO)** | Normally Open | **Headlamp Module**| Low-Beam Pin | Engages dimming loops only when signaled by software  |

---

## 💻 Software Implementation & Calibration Thresholds

The software layout executes a non-blocking polling sequence written in C++ within the Arduino IDE. To ensure distinct state isolation, a pull-down resistor optimization study was performed. The system incorporates a 2.2 kΩ resistor for the feedback LDR voltage divider, widening the voltage separation between the headlight's compressed operational beam profiles.

### Programmable Operational Bounds
Data-driven software decisions evaluate light configurations as follows:

| Environmental Lux State | Verified Lens Range (ADC)  | Registered Software State  | Programmatic Trigger Action  |
| :--- | :--- | :--- | :--- |
| Dark Baseline | `0 to 20` | `OFF` | Default dark system baseline (`feedbackDimMax = 220`) |
| Dim Luminous Profile | `21 to 499` | `DIM BEAM` | Activates localized town illumination (`feedbackLowMax = 500`) |
| Low Beam Profile | `500 to 619` | `LOW BEAM` | Drops intensity automatically to counter oncoming traffic glare (`feedbackHighMax = 620`) |
| High Beam Profile | `620 to 1023` | `HIGH BEAM` | Full operational intensity; maximum safe distance visibility |

*A 250ms software delay is implemented between validation reads to allow high-current lighting states to fully stabilize, avoiding false triggers due to sudden voltage fluctuations during switching.*

---

## 🔬 Validation Protocols & Test Results

Validation was conducted through iterative laboratory simulations to ensure the system safely handles transition logic and adapts correctly to sudden changes in illumination.

### Diagnostic Test Phase 1: Open-Loop Blind Testing
* **Protocol:** The system was initialized with the ambient LDR (A0) covered to mandate high-beam output, then flashed with external light sources to force a low-beam adjustment state.
* **Observation:** The serial interface logged calculations correctly. However, if a relay pulse was dropped due to minor contact latency, or if manual adjustments occurred over the physical lamp, the software continued tracking out-of-phase configurations. The controller became blind to true hardware states, leaving the software out of sync.

### Diagnostic Test Phase 2: Closed-Loop Dynamic Tracking Validation
* **Protocol:** The headlamp was intentionally uncoupled before boot-up to create a severe state misalignment under total dark baseline criteria.
* **Observation:** Upon boot-up, the system immediately intercepted the state misalignment. Instead of stalling, the code initialized its active correction routine, pulsing the Active-LOW relay module sequentially. It evaluated real-time loop feedback every 250ms, stepping directly through `OFF` (ADC: 85), `DIM BEAM` (ADC: 446), and `LOW BEAM` (ADC: 570). On the fourth command pulse, the lens reached `ADC: 676`. The Arduino verified this value as `HIGH BEAM`, exited the correction loop, and safely paused further relay actions.

### Environmental Resilience Summary
By housing the internal feedback LDR within an optically isolated capture cylinder, interference from ambient overhead laboratory light changes was eliminated. The 250ms delay allowed high-current headlight circuitry to fully stabilize before analog sampling occurred, preventing false switches caused by standard automotive switching voltage fluctuations.

---

## 🔮 Future Research & Enhancements

* **Custom Printed Circuit Boards (PCBs):** Transitioning away from modular breadboards toward dedicated custom PCBs to reduce system bulkiness, eliminate loose connection points, simplify installation, and withstand rough road vibrations.
* **Intelligent Camera Integration:** Upgrading baseline photoresistor arrays to compact camera modules running edge-based machine vision to actively differentiate vehicle light signatures from static street illumination.
* **Solid-State Power Regulation:** Swapping out mechanical electromechanical relays for high-speed MOSFET components to implement smooth, continuous dimming instead of abrupt switching.
---

## 👥 Project Contribution & Credits

### SmartBeam Development Team
* **Kabagyo Desire Acquilina** (Reg No: 23/U/1626/GCS/PS) — Gulu University 
* **Lanyero Fortunate** (Reg No: 23/U/3718/GCS/PS) — Gulu University 
* **Akejo Ivan Gira** (Reg No: 23/U/1605/GCS/PS) — Gulu University 

### Academic Supervision & Mentorship
* **Supervisor:** Assoc. Prof. Benedict Oyo (Department of Computer Science, Gulu University)
* **Technical Mentor:** Mr. Allan Junior Asiku

---
### 📄 Reference Document Citation
All structural parameters, systemic design limits, and testing evaluations documented in this repository are verified against the capstone final report file verbatim: **`SMART_BEAM1 (3).pdf`**.

```
