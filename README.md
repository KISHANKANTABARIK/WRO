# Engineering Documentation - Team DeusMachina

![Team Banner](./t-photos/team-banner.jpeg)

## Introduction

In this repository you will find all the information related to the construction, design, and programming of the autonomous robot built by *Team DeusMachina* for the  Future Engineers category in the 2026 Season.
This has been an incredible journey with a lot of obstacles, and we have done our best to come up with a design that we were not only satisfied with, but one that could properly accomplish all of the objectives present in this edition of the competition.

## Contents

- [Team Members](#team-members)
- [Hardware Design](#hardware-design)
  * [Design Process](#design-process)
  * [Assembly Process](#assembly-process)
  * [Elements](#elements)
    + [3D Printed](#3d-printed)
    + [Electronics](#electronics)
- [Software Design](#software-design)
  * [Image and Sensor Processing](#image-and-sensor-processing)
    + [Image Capturing](#image-capturing)
    + [Processing Pipeline](#processing-pipeline)
    + [Distance / Position Detection](#distance--position-detection)
  * [Robot Movement](#robot-movement)
    + [Steering / Servo Configuration](#steering--servo-configuration)
    + [Robot Orientation](#robot-orientation)
    + [Route Determination](#route-determination)
  * [Data Sending](#data-sending)
- [Impact](#impact)
- [Our Journey](#our-journey)

---

# Team Members

[#team-members](#team-members)

- **[Kishan Kanta Barik]**:

![Member 1 photo](./t-photos/member1.jpg)
> Hi, my name is Kishan Kanta Barik. I'm part of Team DeusMachina and I'm responsible for ROS2 Architecture & Embedded C . 

- **[Swastik Prasad Behera]**:

![Member 2 photo](./t-photos/member2.jpeg)
> My name is Swastik Prasad Behera. I am the team's Designer and I'm responsible for Mechanical Designing.

- **[Member Name 3]**:

![Mohit Kumar Bera](./t-photos/member3.jpeg)
> I am Mohit Kumar Bera, the team's Electronics Engineering. I am in charge of Electronics circuits and wiring. 

---

# Hardware Design

## Design Process

Describe the overall hardware architecture here: what the robot is composed of (3D printed parts, off-the-shelf components, chassis type, drivetrain, etc.), and the key design decisions made along the way.

> Example: *The robot is composed of a combination of 3D printed parts for most of its structure, [wheel type], and the electronics that give the robot its functionality.*

Document major design iterations/changes chronologically, each with a short explanation of **what changed** and **why**:

1. **[Change 1 — e.g. Motor upgrade]**: Explain the problem with the old approach and why the new component/approach was chosen.

![Design change 1](./schemes/design-change-1.png)

2. **[Change 2 — e.g. Wheel replacement]**: Explain the reasoning.

![Design change 2](./schemes/design-change-2.png)

3. **[Change 3 — e.g. Component reorganization]**: Explain the reasoning.

![Design change 3](./schemes/design-change-3.png)

Describe the final architecture (e.g. modular design with distinct functional layers):

> Example: *The final design of the robot is intended as a modular design, consisting of N modules: [module 1 name/purpose], [module 2 name/purpose], [module 3 name/purpose]. This makes it easier to make repairs or replace/rearrange parts.*

![Final structure](./schemes/final-structure.pdf)

Link to the full 3D model/CAD files:
[3D design files](./models)

## Assembly Process

Walk through how the robot was physically assembled, module by module or subsystem by subsystem. Include any problems encountered during assembly/testing and how they were solved (e.g. sensor placement issues, interference, structural weaknesses).

Include a wiring/assembly diagram if available:

![Assembly diagram](./schemes/assembly-diagram.png)

## Elements

### 3D Printed

> **Note:** State your print settings here (material, layer height, infill %, etc.)

- **[Part Name 1]**: Description of its function.

![Part 1](./v-photos/part1.png)

- **[Part Name 2]**: Description of its function.

![Part 2](./v-photos/part2.png)

- **[Part Name 3]**: Description of its function.

![Part 3](./v-photos/part3.png)

*(Add one entry per printed part — chassis, brackets, sensor mounts, covers, etc.)*

### Electronics

For each component, include a short description of its role and a spec box.

- **[Component 1 — e.g. Motor Driver]**: Role in the robot.

![Component 1](./v-photos/component1.png)

> **Specifications**
> - Spec 1
> - Spec 2
> - Spec 3

- **[Component 2 — e.g. Drive Motor]**: Role in the robot.

![Component 2](./v-photos/component2.png)

> **Specifications**
> - Spec 1
> - Spec 2
> - Spec 3

- **[Component 3 — e.g. Steering Servo]**: Role in the robot.

- **[Component 4 — e.g. Raspberry pi4 - Module B (4GB Ram)]**: Main Micro Processor.

- **[Component 5 — e.g. STM32466RE]**: Secondary microcontroller.

- **[Component 6 — e.g. 2D 360 degree Lidar ]**: To measure and detect the object distance and real time objects.

- **[Component 7 — e.g. Camera]**: Identify the colour of th object.

- **[Component 8 — e.g. Batteries]**: Power source of the vehicle.

*(Add/remove entries to match your actual bill of materials.)*

---

# Software Design

## Image and Sensor Processing

### Image Capturing

Describe how frames/images are captured (camera model, resolution, library used, e.g. OpenCV) and at what stage in the pipeline this happens.

### Processing Pipeline

Describe your detection/processing pipeline step by step (e.g. color space conversion, masking, contour detection). Include representative code snippets:

```python
# Example: HSV threshold values for detection
lower_bound_1 = np.array([H_min, S_min, V_min], np.uint8)
upper_bound_1 = np.array([H_max, S_max, V_max], np.uint8)
```

![Processing example](./schemes/processing-example.png)

### Distance / Position Detection

Explain how the robot determines distance/position of detected objects relative to itself (e.g. dividing the frame into zones, using sensor fusion, etc.)

![Position detection diagram](./schemes/position-diagram.png)

## Robot Movement

### Steering / Servo Configuration

Describe how the steering mechanism is controlled (pin assignments, control functions/library calls) and how sensor input maps to steering commands.

### Robot Orientation

Describe how the robot determines and maintains its orientation/heading (e.g. based on first detected turn direction, gyroscope, encoders).

### Route Determination

Describe the logic that decides the robot's path (e.g. based on detected color/marker, sensor readings, or map data) and how that decision is communicated between subsystems.

## Data Sending

Describe the communication protocol between your controllers/boards (e.g. I2C, UART, SPI) — include the libraries used, addressing, and wiring (e.g. SDA, SCL, GND for I2C).

```python
# Example: initializing I2C communication
import smbus
bus = smbus.SMBus(1)
I2C_ADDRESS = 0x08
```

---

# Impact

Reflect on the goals of the project, the skills your team developed (technical and non-technical), and the challenges you overcame. This section should convey the learning outcomes and personal/team growth from the project.

---

# Our Journey

Tell your team's story: how you got started, key milestones, competitions attended, results achieved, and what you're aiming for next. This is the narrative/human section of the documentation — make it personal.

---

# Thank You. Team DeusMachina - 2026.

## Repository Structure

```
.
├── models/         # 3D design/CAD files
├── other/          # Miscellaneous supporting files
├── schemes/        # Diagrams, wiring schematics, flowcharts
├── src/            # Source code (robot firmware/software)
├── t-photos/       # Team photos
├── v-photos/       # Vehicle/robot photos
├── videos/         # Demonstration videos
├── README.md       # This file
└── Team-Report_[Competition-Name].pdf   # Official team report
```
