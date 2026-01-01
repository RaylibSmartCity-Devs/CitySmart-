# 🚦 Smart City Traffic Simulation

A 3D simulation of a smart city traffic network built with **C++** and **Raylib**. This project demonstrates intelligent pathfinding, traffic light synchronization, and autonomous yielding behaviors for emergency vehicles.

## 🎥 Demo

DEMO VID TO ADD

## ✨ Key Functionalities

### 1. 🚑 Emergency Vehicle Priority System
* **Pathfinding:** The ambulance calculates the fastest route to an incident using A* (or Dijkstra) algorithms.
* **Siren Logic:** When the siren is active, traffic lights dynamically turn green for the ambulance.
* **Smart Yielding:** Civilian cars detect the ambulance from a distance and "snap" to the sidewalk (Solid Translation) to clear the road.
* **Safe Interaction:** Cars wait on the sidewalk until the ambulance passes before re-entering traffic.

### 2. 🚦 Intelligent Traffic Lights
* **Desynchronized Cycles:** Traffic lights start at random time offsets to prevent city-wide gridlocks.
* **Realistic Phases:** Lights cycle through Green -> Yellow -> Red with standard timings.
* **Emergency Override:** Lights automatically switch to green when an emergency vehicle approaches.

### 3. 🚗 Autonomous Traffic
* **Lane Following:** Vehicles stay strictly in the right lane using calculated offsets.
* **Collision Avoidance:** Cars detect vehicles ahead and maintain safe braking distances.
* **Intersection Safety:** Vehicles obey traffic rules and stop at red lights.

### 4. 🏙️ 3D City Rendering
* **Detailed Environment:** Renders roads, intersections, residential/commercial buildings, and facilities (Hospitals, Police Stations).
* **Custom Models:** Vehicles are rendered with chassis, cabin, glass, wheels, and headlights/taillights.
* **Camera Controls:** Fully interactive 3D orbit camera.

---

## 🎮 Controls

| Key / Input | Action |
| :--- | :--- |
| **W / S** | Zoom Camera In / Out |
| **A / D** | Rotate Camera Left / Right |
| **Left Click** | Set Emergency Destination (Green Path) |
| **G** | **GO!** Start Ambulance Mission |
| **Space** | Pause / Resume Simulation |

---

## 🛠️ How to Run

### Prerequisites
* C++ Compiler (GCC/Clang/MSVC)
* CMake (Version 3.10+)
* Raylib (Installed via vcpkg or system libraries)

### Build Instructions

1.  **Clone the Repository**
    ```bash
    git clone [https://github.com/RaylibSmartCity-Devs/CitySmart-.git](https://github.com/RaylibSmartCity-Devs/CitySmart-.git)
    cd CitySmart-
    ```

2.  **Create Build Directory**
    ```bash
    mkdir build
    cd build
    ```

3.  **Compile the Project**
    ```bash
    cmake ..
    cmake --build .
    ```

4.  **Run the Simulation**
    * **Windows:** `.\Debug\CitySmart.exe` (or just `.\CitySmart.exe`)
    * **Linux/Mac:** `./CitySmart`

---

## 📂 Project Structure

* `src/CityMap.cpp`: Map generation, tile logic, and texture loading.
* `src/Vehicle.cpp`: Base class for civilian cars, movement physics, and yielding logic.
* `src/EmergencyVehicle.cpp`: Specialized logic for the ambulance and sirens.
* `src/TrafficLight.cpp`: Logic for light cycles and emergency overrides.
* `assets/`: Contains textures for buildings, roads, and environment.

---

## 👥 Authors

* **Ayoub Chentouf** - *Lead Developer*
* *(Add your team members here)*
