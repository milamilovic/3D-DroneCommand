# DroneCommand

DroneCommand is an interactive control station for managing unmanned aerial vehicles (UAVs), featuring real-time map visualization, flight control, battery monitoring, no-fly zone management, and collision detection for seamless and safe drone operations.


## Screenshots

### Models
![Drone model](https://github.com/user-attachments/assets/f9c5b8fc-6a81-4282-8271-e27683d101b5)

![Majevica model with google images](https://github.com/user-attachments/assets/22831883-2242-4295-9395-ea049dc4faae)

![Majevica model with the topographic map](https://github.com/user-attachments/assets/e9c6ad7c-8859-4f59-8437-04310c602d06)


### Application
![Starting screen with turned off cameras](https://github.com/user-attachments/assets/efe89f46-6145-4ba5-ac83-1b3ae9928008)

![Screen with camera viewes](https://github.com/user-attachments/assets/3aec547e-6b57-48cf-bce0-ca06b91c95b1)


## Features

### Core Components

- **LED Status Indicators**: Real-time visual feedback for UAV activity status
- **Topographic Map**: Detailed representation of Mount Majevica and surrounding municipalities with technological glass overlay
- **No-Fly Zone Management**: Interactive zone control with drag-and-drop functionality
- **Battery Monitoring**: Real-time battery level tracking with coordinate display
- **Dual UAV Control**: Simultaneous control of two independent drones
- **Collision Detection**: Automatic safety measures for out-of-bounds, collision, and low-battery scenarios

### Interactive Elements

#### LED Indicators
- **Dark**: UAV inactive
- **Bright**: UAV active

#### Topographic Map
- Textured representation of Mount Majevica region
- Greenish glass overlay for enhanced technological aesthetic
- Real-time UAV position tracking

#### No-Fly Zone
- **Location**: Positioned over Lopare municipality
- **Appearance**: Semi-transparent circular overlay
- **Interactive Controls**:
  - **Move**: Left-click and drag to reposition
  - **Resize**: Right-click and hold to expand
  - **Reset**: Press `R` to restore original size and position
- **Safety Feature**: Automatically destroys UAVs that enter the zone

#### Battery Management
- Dual progress bars showing battery percentage
- Real-time coordinate display (X, Y positions)
- Gradual battery depletion during flight
- Automatic UAV destruction when battery depleted

## Controls

### UAV Activation/Deactivation
| Action | Keys | Description |
|--------|------|-------------|
| Activate UAV 1 | `1` + `U` | Powers on first drone |
| Activate UAV 2 | `2` + `U` | Powers on second drone |
| Deactivate UAV 1 | `1` + `I` | Powers off first drone |
| Deactivate UAV 2 | `2` + `I` | Powers off second drone |

### UAV Movement
| Action | Keys | Description |
|--------|------|-------------|
| Control UAV 1 | `W` `A` `S` `D` | Move first drone (when active) |
| Control UAV 2 | `↑` `↓` `←` `→` | Move second drone (when active) |

### No-Fly Zone Management
| Action | Control | Description |
|--------|---------|-------------|
| Move Zone | Left Mouse + Drag | Relocate no-fly zone |
| Resize Zone | Right Mouse + Hold | Expand zone size |
| Reset Zone | `R` | Restore original position and size |

## Safety Features

### Automatic UAV Destruction
UAVs are automatically destroyed and permanently deactivated when:
- Colliding with each other
- Moving outside the map boundaries
- Entering the no-fly zone
- Battery completely depleted
- Encountered during no-fly zone repositioning or resizing

### Visual Feedback
- Destruction notifications appear on screen when UAVs are destroyed
- Real-time battery and position monitoring

## Getting Started

1. Clone the repository
2. Launch the application
3. Both UAVs start grounded with full batteries
4. Use the control keys to activate and maneuver the drones
5. Monitor battery levels and avoid the no-fly zone
6. Manage the no-fly zone position and size as needed

## Technical Details

- **Map Region**: Mount Majevica and surrounding municipalities
- **UAV Representation**: Circular indicators with real-time positioning
- **Battery System**: Gradual depletion with percentage display
- **Collision System**: Multi-condition safety checks
- **Interactive Controls**: Mouse and keyboard input handling

## Author

**Mila Milović**  
Student ID: SV22-2021

## License

This project is part of an academic assignment and is intended for educational purposes.
