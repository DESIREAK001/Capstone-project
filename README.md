# SmartBeam: An Embedded Program for Automatic Headlight Intensity Adjustment

SmartBeam is an affordable automotive retrofit solution designed to automatically modulate vehicle headlight intensity in real time. By dynamically switching between high beam, low beam, and intermediate states based on oncoming traffic and ambient lighting, the system mitigates the dangerous effects of headlight glare, reduces glare-induced driver fatigue, and enhances night-time road safety in resource-constrained environments.

[cite_start]This system was designed, built, and validated as part of a capstone project for the partial fulfillment of the award of a Bachelor's Degree of Science in Computer Science at Gulu University[cite: 11]. All technical parameters, circuit constraints, and system designs referenced here are derived from the project report file named **"SMART_BEAM1 (3).pdf"**.

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

[cite_start]Night-time driving is a necessity for long-distance transport, yet visibility remains heavily compromised by irresponsible driving habits and inadequate technological adoption[cite: 79, 80]. [cite_start]Approximately half of all traffic deaths occur during night-time hours (between 8:00 p.m. and 11:59 p.m.)[cite: 85, 86]. [cite_start]A primary catalyst for these accidents is temporary blindness and reduced reaction times caused by the blinding glare of high beams from oncoming vehicles[cite: 81, 132].

[cite_start]While high-end modern vehicles feature factory-installed adaptive high-beam assist or matrix LED frameworks, these vehicles likely constitute less than 5% of the active fleet on Ugandan roads[cite: 101, 103]. [cite_start]The remaining 95% consists of older passenger sedans and SUVs that lack automated glare-control architectures[cite: 103, 113]. [cite_start]SmartBeam bridges this safety gap by delivering an affordable, context-specific retrofit package engineered using accessible embedded hardware and a closed-loop control system[cite: 67, 71, 644].

---

## 🎯 System Objectives

* **Investigate Limitations:** Evaluate the gaps and strengths of baseline automatic lights, adaptive headlights, high-beam assist, and matrix illumination systems[cite: 108].
* [cite_start]**Design an Embedded Program:** Develop logical, low-latency code mapping physical light data to deterministic hardware states using advanced sensor technology[cite: 109].
* [cite_start]**Implement Prototype:** Assemble a bench-top, closed-loop functional prototype capable of simulating automotive beam transformations safely[cite: 110, 644].
* **Validate Performance:** Test response times, environmental resilience, and loop stability within a controlled laboratory environment[cite: 111, 590].

---

## 🏗️ System Architecture & Logical Design

The system relies on a **closed-loop feedback control architecture** designed to actively intercept and correct real-time hardware desynchronization[cite: 644].

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
* [cite_start]**Sensing Transformation:** Process 1.0 measures ambient lux via a front-facing sensor array, while Process 2.0 reads continuous luminosity directly from an isolated sub-section of the vehicle's headlight lens, digitizing both parameters into 10-bit values (0 to 1023)[cite: 441, 442, 443, 444].
* [cite_start]**Serialization & Buffer:** Readings are cached in temporary registers (Data Store D1 for Ambient, Data Store D2 for Feedback) to decouple hardware speeds from software execution[cite: 445, 446].
* [cite_start]**Central Processing Engine:** Process 3.0 parses the data, routing output vectors concurrently to a hardware evaluation matrix and system diagnostics via terminal serialization[cite: 448, 449, 450].
* [cite_start]**Target State Determination:** Process 4.0 cross-references ambient values against limits stored in Data Store D3 (`State Configuration`)[cite: 451, 453]. [cite_start]Process 5.0 checks this computed target against the active profile in Data Store D4 (`Current Verified State`)[cite: 451, 454]. [cite_start]If states match, execution ends[cite: 455].
* [cite_start]**Actuation Correction:** Upon detecting a divergence, Process 6.0 sends a precise, 150ms Active-LOW pulse out to the physical Relay Module[cite: 456, 457]. [cite_start]This forces a sequence shift, which is instantly captured and verified by the local feedback loop, locking down stability[cite: 457, 458].

---

## 🔌 Hardware Specification & Wiring Layout

[cite_start]The bench-top prototype was developed using durable, high-availability electrical components to simulate an automotive power network[cite: 554].

### Component Wiring Matrix
[cite_start]To protect processing logic from vehicle voltage disruptions, wiring paths were executed based on the following structural pin maps[cite: 556]:

| Source Component | Pin Out Reference | Target Component | Pin In Reference | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| **Buck Regulator** | Vout AWG Solid Jumper | **Arduino Uno** | 5V Pin22 | [cite_start]Steps vehicle source power down to safe logic tolerances [cite: 560] |
| **LDR Sensor Array** | Signal Out | **Arduino Uno** | Pin A0 | [cite_start]Continuously logs environmental lux levels [cite: 560] |
| **Arduino Uno** | Pin 7 | **Relay Module** | Signal/In | [cite_start]Drives pulse instructions to the physical switch loop [cite: 560] |
| **Relay Output (COM)** | Common | **Battery Vehicle (12V)**| positive (+) | [cite_start]Channels the main illumination system source power [cite: 560] |
| **Relay Output (NC)** | Normally Closed | **Headlamp Module**| High-Beam Pin | [cite_start]Maps to high beams by default to prioritize safety [cite: 538, 560] |
| **Relay Output (NO)** | Normally Open | **Headlamp Module**| Low-Beam Pin | [cite_start]Engages dimming loops only when signaled by software [cite: 560] |

---

## 💻 Software Implementation & Calibration Thresholds

[cite_start]The software layout executes a non-blocking polling sequence written in C++ within the Arduino IDE[cite: 579]. [cite_start]To ensure distinct state isolation, a pull-down resistor optimization study was performed[cite: 581]. [cite_start]The system incorporates a 2.2 kΩ resistor for the feedback LDR voltage divider, widening the voltage separation between the headlight's compressed operational beam profiles[cite: 582].

### Programmable Operational Bounds
[cite_start]Data-driven software decisions evaluate light configurations as follows[cite: 580]:

| Environmental Lux State | [cite_start]Verified Lens Range (ADC) [cite: 584] | [cite_start]Registered Software State [cite: 584] | [cite_start]Programmatic Trigger Action [cite: 584] |
| :--- | :--- | :--- | :--- |
| Dark Baseline | `0 to 20` | `OFF` | Default dark system baseline (`feedbackDimMax = 220`) |
| Dim Luminous Profile | `21 to 499` | `DIM BEAM` | Activates localized town illumination (`feedbackLowMax = 500`) |
| Low Beam Profile | `500 to 619` | `LOW BEAM` | Drops intensity automatically to counter oncoming traffic glare (`feedbackHighMax = 620`) |
| High Beam Profile | `620 to 1023` | `HIGH BEAM` | Full operational intensity; maximum safe distance visibility |

[cite_start]*A 250ms software delay is implemented between validation reads to allow high-current lighting states to fully stabilize, avoiding false triggers due to sudden voltage fluctuations during switching[cite: 610].*

---

## 🔬 Validation Protocols & Test Results

[cite_start]Validation was conducted through iterative laboratory simulations to ensure the system safely handles transition logic and adapts correctly to sudden changes in illumination[cite: 590].

### Diagnostic Test Phase 1: Open-Loop Blind Testing
* [cite_start]**Protocol:** The system was initialized with the ambient LDR (A0) covered to mandate high-beam output, then flashed with external light sources to force a low-beam adjustment state[cite: 593, 594].
* [cite_start]**Observation:** The serial interface logged calculations correctly[cite: 595]. [cite_start]However, if a relay pulse was dropped due to minor contact latency, or if manual adjustments occurred over the physical lamp, the software continued tracking out-of-phase configurations[cite: 596]. [cite_start]The controller became blind to true hardware states, leaving the software out of sync[cite: 597].

### Diagnostic Test Phase 2: Closed-Loop Dynamic Tracking Validation
* [cite_start]**Protocol:** The headlamp was intentionally uncoupled before boot-up to create a severe state misalignment under total dark baseline criteria[cite: 601, 602].
* [cite_start]**Observation:** Upon boot-up, the system immediately intercepted the state misalignment[cite: 603]. [cite_start]Instead of stalling, the code initialized its active correction routine, pulsing the Active-LOW relay module sequentially[cite: 604]. [cite_start]It evaluated real-time loop feedback every 250ms, stepping directly through `OFF` (ADC: 85), `DIM BEAM` (ADC: 446), and `LOW BEAM` (ADC: 570)[cite: 605]. [cite_start]On the fourth command pulse, the lens reached `ADC: 676`[cite: 605]. [cite_start]The Arduino verified this value as `HIGH BEAM`, exited the correction loop, and safely paused further relay actions[cite: 605, 606].

### Environmental Resilience Summary
[cite_start]By housing the internal feedback LDR within an optically isolated capture cylinder, interference from ambient overhead laboratory light changes was eliminated[cite: 608, 609]. [cite_start]The 250ms delay allowed high-current headlight circuitry to fully stabilize before analog sampling occurred, preventing false switches caused by standard automotive switching voltage fluctuations[cite: 610].

---

## 🔮 Future Research & Enhancements

* [cite_start]**Custom Printed Circuit Boards (PCBs):** Transitioning away from modular breadboards toward dedicated custom PCBs to reduce system bulkiness, eliminate loose connection points, simplify installation, and withstand rough road vibrations[cite: 691, 692, 693].
* [cite_start]**Intelligent Camera Integration:** Upgrading baseline photoresistor arrays to compact camera modules running edge-based machine vision to actively differentiate vehicle light signatures from static street illumination[cite: 695].
* [cite_start]**Solid-State Power Regulation:** Swapping out mechanical electromechanical relays for high-speed MOSFET components to implement smooth, continuous dimming instead of abrupt switching[cite: 696].

---

## 👥 Project Contribution & Credits

### SmartBeam Development Team
* [cite_start]**Kabagyo Desire Acquilina** (Reg No: 23/U/1626/GCS/PS) — Gulu University [cite: 7, 8]
* [cite_start]**Lanyero Fortunate** (Reg No: 23/U/3718/GCS/PS) — Gulu University [cite: 9]
* [cite_start]**Akejo Ivan Gira** (Reg No: 23/U/1605/GCS/PS) — Gulu University [cite: 10]

### Academic Supervision & Mentorship
* **Supervisor:** Assoc. [cite_start]Prof. Benedict Oyo (Department of Computer Science, Gulu University) [cite: 19]
* [cite_start]**Technical Mentor:** Mr. Allan Junior Asiku [cite: 35]

---
### 📄 Reference Document Citation
All structural parameters, systemic design limits, and testing evaluations documented in this repository are verified against the capstone final report file verbatim: **`SMART_BEAM1 (3).pdf`**.

```
