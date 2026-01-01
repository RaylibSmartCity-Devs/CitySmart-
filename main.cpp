#include "raylib.h"
#include "raymath.h"
#include "rlgl.h" 
#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>   
#include <algorithm>
#include <cmath>
#include "GameCommon.h"   
#include "CityMap.h"
#include "Vehicle.h"
#include "EmergencyVehicle.h"
#include "TrafficLight.h"

#define TILE_SIZE 4.0f

// =====================================================
// SIMULATION CLASS
// =====================================================
class Simulation {
private:
    EmergencyVehicle* ambulance;
    std::vector<Vehicle*> normalTraffic; 
    
   
    std::vector<Vector3> lastPos;
    std::vector<float> stuckTime;
CityMap* cityMap;
    bool isMoving;   
    std::vector<Vector3> previewPath;
    bool ambulanceMoving = false; 
    Vector3 hospitalPos;
    bool isReturning;
    std::vector<Vector3> outboundPath;

    std::vector<TrafficLight> lights;

    enum class MissionState { Idle, ToIncident, ToHospital };
    MissionState mission{MissionState::Idle};
    Vector3 incidentPos{0,0,0};


public:
    // Helper to get centered tile coordinates
    Vector3 Tile(int x, int y) {
        return { (float)x * TILE_SIZE + 2.0f, 0, (float)y * TILE_SIZE + 2.0f };
    }

    Simulation(CityMap* map) {
        cityMap = map;
        srand((unsigned int)time(NULL));
        // Find the hospital tile in the map (portable: no hardcoded coords)
        hospitalPos = Tile(8, 6); // fallback
        for (int y = 0; y < CityMap::H; y++) {
            for (int x = 0; x < CityMap::W; x++) {
                if (cityMap->facilityMap[y][x] == HOSPITAL) {
                    hospitalPos = cityMap->tileCenter(y, x);
                    y = CityMap::H; // break both loops
                    break;
                }
            }
        }
        isReturning = false;
        
        
        ambulance = new EmergencyVehicle(hospitalPos, {0,0,0}, 5.0f, RED, "Ambulance");
    
   
        ambulance->toggleSiren(true);
    
    
        ambulance->setLaneOffset(0.2f); 
        ambulance->setLooping(false);
        InitHardcodedTraffic();
        InitTrafficLights();
        for (size_t i = 0; i < normalTraffic.size(); i++) {
    normalTraffic[i]->setLaneOffset(0.6f); 
}
        
        // Init stuck trackers
        lastPos.clear();
        stuckTime.clear();
        for (auto v : normalTraffic) { lastPos.push_back(v->getPosition()); stuckTime.push_back(0.0f); }
isMoving = false; 
    }

    ~Simulation() { 
        delete ambulance; 
        for(auto v : normalTraffic) delete v;
    }

    // PATHS aligned with the new 10x10 road network
    std::vector<Vector3> GetTopLapPath() {
        std::vector<Vector3> fullPath;
        std::vector<Vector3> segment;
        segment = cityMap->findFastestPath(Tile(6, 0), Tile(6, 2), false);
        fullPath.insert(fullPath.end(), segment.begin(), segment.end());
        segment = cityMap->findFastestPath(Tile(6, 2), Tile(9, 2), false);
        fullPath.insert(fullPath.end(), segment.begin(), segment.end());
        segment = cityMap->findFastestPath(Tile(9, 2), Tile(6, 2), false);
        fullPath.insert(fullPath.end(), segment.begin(), segment.end());
        segment = cityMap->findFastestPath(Tile(6, 2), Tile(6, 0), false);
        fullPath.insert(fullPath.end(), segment.begin(), segment.end());
        return fullPath;
    }

    std::vector<Vector3> GetBottomLapPath() {
        std::vector<Vector3> fullPath;
        std::vector<Vector3> segment;
        segment = cityMap->findFastestPath(Tile(0, 9), Tile(4, 9), false);
        fullPath.insert(fullPath.end(), segment.begin(), segment.end());
        segment = cityMap->findFastestPath(Tile(4, 9), Tile(9, 9), false);
        fullPath.insert(fullPath.end(), segment.begin(), segment.end());
        return fullPath;
    }

    void SetupLoopingCar(int index) {
        Vehicle* v = nullptr;
        if (index == 0) {
            v = new Vehicle(Tile(6, 0), Tile(6, 0), 3.5f, BLUE);
            v->setPath(GetTopLapPath());
        }
        else if (index == 1) {
            v = new Vehicle(Tile(0, 9), Tile(0, 9), 4.0f, GREEN);
            v->setPath(GetBottomLapPath());
        }
        
        if (v != nullptr) {
            if (index >= (int)normalTraffic.size()) normalTraffic.push_back(v);
            else { delete normalTraffic[index]; normalTraffic[index] = v; }
        }
    }

    
    void InitTrafficLights() {
    lights.clear();

    for (int y = 0; y < CityMap::H; y++) {
        for (int x = 0; x < CityMap::W; x++) {
            int t = cityMap->tileMap[y][x];

            if (t == INTERSECTION || t == TROAD || t == TROAD1 || t == ROUNDABOUT || t == ROTROAD) {
                Vector3 p = cityMap->tileCenter(y, x);
                p.y = 0.0f;
                lights.emplace_back(p, 3.0f); 
            }
        }
    }
}
void InitHardcodedTraffic() {
        normalTraffic.clear();

        // ROUTE A: THE GRAND TOUR (Tall Loop)
        Vector3 t1 = Tile(4, 2); 
        Vector3 t2 = Tile(6, 2);
        Vector3 t3 = Tile(6, 9);
        Vector3 t4 = Tile(4, 9);

        std::vector<Vector3> longPath;
        std::vector<Vector3> segment;

        segment = cityMap->findFastestPath(t1, t2, false);
        longPath.insert(longPath.end(), segment.begin(), segment.end()); 
        segment = cityMap->findFastestPath(t2, t3, false);
        longPath.insert(longPath.end(), segment.begin(), segment.end()); 
        segment = cityMap->findFastestPath(t3, t4, false);
        longPath.insert(longPath.end(), segment.begin(), segment.end()); 
        segment = cityMap->findFastestPath(t4, t1, false);
        longPath.insert(longPath.end(), segment.begin(), segment.end()); 

        if (!longPath.empty()) {
            Vehicle* v1 = new Vehicle(longPath[0], {0,0,0}, 4.0f, BLUE);
            v1->setPath(longPath);
            v1->setLooping(true);
            normalTraffic.push_back(v1);

            int half = (int)longPath.size() / 2;
            if (longPath.size() > half) {
                std::vector<Vector3> p2 = longPath;
                std::rotate(p2.begin(), p2.begin() + half, p2.end());
                Vehicle* v2 = new Vehicle(p2[0], {0,0,0}, 4.2f, GREEN);
                v2->setPath(p2);
                v2->setLooping(true);
                normalTraffic.push_back(v2);
            }
        }

        // ROUTE B: THE CITY CENTER (Short Loop)
        Vector3 s1 = Tile(4, 2);
        Vector3 s2 = Tile(6, 2);
        Vector3 s3 = Tile(6, 7); 
        Vector3 s4 = Tile(4, 7);

        std::vector<Vector3> shortPath;
        
        segment = cityMap->findFastestPath(s1, s2, false);
        shortPath.insert(shortPath.end(), segment.begin(), segment.end()); 
        segment = cityMap->findFastestPath(s2, s3, false);
        shortPath.insert(shortPath.end(), segment.begin(), segment.end()); 
        segment = cityMap->findFastestPath(s3, s4, false);
        shortPath.insert(shortPath.end(), segment.begin(), segment.end()); 
        segment = cityMap->findFastestPath(s4, s1, false);
        shortPath.insert(shortPath.end(), segment.begin(), segment.end()); 

        if (!shortPath.empty()) {
            Vehicle* v3 = new Vehicle(shortPath[0], {0,0,0}, 3.8f, YELLOW);
            v3->setPath(shortPath);
            v3->setLooping(true);
            normalTraffic.push_back(v3);

            int half = (int)shortPath.size() / 2;
            if (shortPath.size() > half) {
                std::vector<Vector3> p4_path = shortPath;
                std::rotate(p4_path.begin(), p4_path.begin() + half, p4_path.end());
                Vehicle* v4 = new Vehicle(p4_path[0], {0,0,0}, 3.5f, ORANGE);
                v4->setPath(p4_path);
                v4->setLooping(true);
                normalTraffic.push_back(v4);
            }
        }
    }

    void Update(Camera3D camera) {
        float dt = GetFrameTime();
        for (auto &l : lights) {
            l.update(dt);
            if (ambulanceMoving && ambulance->getSirenActive()) {
                l.greenForEmergencyIfApproaching(ambulance->getPosition(), ambulance->getForwardDir());
            }
        }

        // 1. Mouse Input
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetMouseRay(GetMousePosition(), camera);
            RayCollision collision = GetRayCollisionQuad(ray, {0,0,0}, {0,0,40}, {40,0,40}, {40,0,0});
            
            if (collision.hit) {
                previewPath = cityMap->findFastestPath(ambulance->getPosition(), collision.point, true);
                ambulanceMoving = false; 
                isReturning = false; 
            }
        }

        // 2. Start Mission
        if (IsKeyPressed(KEY_G) && !previewPath.empty()) {
            outboundPath = previewPath;
            ambulance->setPath(previewPath);
            ambulanceMoving = true;
            isReturning = false;
            mission = MissionState::ToIncident;
            if (!outboundPath.empty()) incidentPos = outboundPath.back();
            previewPath.clear();
        }

        if (IsKeyPressed(KEY_SPACE)) isMoving = !isMoving;

        // 3. Main Loop
        if (isMoving) {
            if (ambulanceMoving) {
                // Prevent the ambulance from driving through cars (wait until they yield)
                bool blocked = false;
                Vector3 aPos = ambulance->getPosition();
                Vector3 aFwd = Vector3Subtract(ambulance->getDestination(), ambulance->getPosition());
                if (Vector3Length(aFwd) > 0.001f) aFwd = Vector3Normalize(aFwd); else aFwd = ambulance->getForwardDir();
                for (auto v : normalTraffic) {
                    Vector3 rel = Vector3Subtract(v->getPosition(), aPos);
                    float forward = Vector3DotProduct(rel, aFwd);
                    if (forward <= 0.0f || forward > 1.4f) continue;
                    Vector3 lateralVec = Vector3Subtract(rel, Vector3Scale(aFwd, forward));
                    float lateral = Vector3Length(lateralVec);
                    if (lateral < 0.75f) { blocked = true; break; }
                }
                if (!blocked) {
                    // Slow down a bit if a car is directly blocking the ambulance (prevents "ghosting" through cars)
                float ambScale = 1.0f;
                Vector3 ambDir = ambulance->getIntendedDir();
                for (auto *car : normalTraffic) {
                    Vector3 rel = Vector3Subtract(car->getPosition(), ambulance->getPosition());
                    float forward = Vector3DotProduct(rel, ambDir);
                    if (forward <= 0.0f || forward > 1.4f) continue;
                    Vector3 latV = Vector3Subtract(rel, Vector3Scale(ambDir, forward));
                    float lateral = Vector3Length(latV);
                    if (lateral < 0.75f) { ambScale = 0.15f; break; }
                }
                ambulance->setSpeedScale(ambScale, 0.12f);
                ambulance->step(dt, true);
                if (!cityMap->isDriveableWorld(ambulance->getPosition())) {
                    ambulance->setPosition(cityMap->clampToDriveable(ambulance->getPosition()));
                }
                }
if (ambulance->hasFinishedPath()) {
                    if (mission == MissionState::ToIncident) {
                        // After reaching the incident, go to the hospital
                        std::vector<Vector3> toHosp = cityMap->findFastestPath(ambulance->getPosition(), hospitalPos, true);
                        if (!toHosp.empty()) {
                            ambulance->setPath(toHosp);
                            mission = MissionState::ToHospital;
                        } else {
                            ambulanceMoving = false;
                            mission = MissionState::Idle;
                            ambulance->toggleSiren(false);
                        }
                    } else if (mission == MissionState::ToHospital) {
                        ambulanceMoving = false;
                        mission = MissionState::Idle;
                        ambulance->toggleSiren(false);
                    } else {
                        ambulanceMoving = false;
                    }
                    isReturning = false;
                }
            }

            // GENTLE YIELDING LOGIC
                        for (size_t i = 0; i < normalTraffic.size(); i++) {
                Vehicle* v = normalTraffic[i];

                // Intended direction (stable even when stopped)
                Vector3 fwd = v->getIntendedDir();

                // 1) Yield to the ambulance (shift within the lane, no drifting into buildings)
                // 1) Yield to the ambulance
       if (ambulanceMoving) {
        Vector3 ambDir = ambulance->getIntendedDir();
        Vector3 rel = Vector3Subtract(v->getPosition(), ambulance->getPosition());
        float dist = Vector3Length(rel);
        float ahead = Vector3DotProduct(rel, ambDir);

        // A. INTERSECTION SAFETY (Frozen if very close to intersection crash)
        int ay, ax;
        cityMap->worldToTile(ambulance->getPosition(), ay, ax);
        int aTile = cityMap->tileMap[ay][ax];
        bool ambInIntersection = (aTile == INTERSECTION || aTile == ROUNDABOUT || 
                                  aTile == TROAD || aTile == TROAD1 || aTile == ROTROAD);

        if (ambInIntersection && dist < 5.0f && ahead > -2.0f) {
             v->setSpeedScale(0.0f, 0.1f); // STOP immediately
        }
        

        else if (dist < 25.0f && ahead > -4.0f && ahead < 20.0f) {
            
            
            v->yieldTo(ambulance->getPosition(), ambDir, 10.0f, 30.0f);
            
            // Check if we are safely on the shoulder
            float myOffset = fabsf(v->getLaneOffset());

            if (myOffset > 1.3f) {
                // WE ARE SAFE: Stop completely
                v->setSpeedScale(0.0f, 0.1f); 
            } else {
                // NOT SAFE YET: 
                // CHANGE 3: Don't slow down! Keep speed at 0.9f (90%) 
                // This allows them to drive *sideways* faster.
                v->setSpeedScale(0.9f, 0.1f); 
            }
        }
    }
                   // 2) Stop at red lights
                   bool stopForRed = false;
                for (const auto& l : lights) {
                    if (l.shouldStop(v->getPosition(), fwd)) { stopForRed = true; break; }
                }

                // 3) Follow the car IN FRONT (same direction + same lane)
                bool stopForCars = false;
                const float followDist = 2.6f;
                const float laneWidth  = 0.65f;
                for (size_t j = 0; j < normalTraffic.size(); j++) {
                    if (i == j) continue;

                    Vector3 otherDir = normalTraffic[j]->getIntendedDir();
                    if (Vector3DotProduct(otherDir, fwd) < 0.65f) continue; // ignore opposite/crossing cars

                    Vector3 rel = Vector3Subtract(normalTraffic[j]->getPosition(), v->getPosition());
                    float forward = Vector3DotProduct(rel, fwd);
                    if (forward <= 0.0f || forward > followDist) continue;

                    Vector3 lateralVec = Vector3Subtract(rel, Vector3Scale(fwd, forward));
                    float lateral = Vector3Length(lateralVec);
                    if (lateral < laneWidth) { stopForCars = true; break; }
                }

                bool allowMove = (!stopForRed && !stopForCars);

                // Mild deadlock recovery (only when NOT waiting at a red light)
                if (i < stuckTime.size()) {
                    float moved = Vector3Distance(v->getPosition(), lastPos[i]);
                    if (moved < 0.01f) stuckTime[i] += dt; else stuckTime[i] = 0.0f;
                    lastPos[i] = v->getPosition();

                    if (!stopForRed && stuckTime[i] > 2.0f) {
                        // tiny nudge forward + small lane nudge to break perfect overlaps
                        allowMove = true;
                        v->nudgeLaneOffset((v->getLaneOffset() >= 0.0f) ? 0.70f : -0.70f, 0.35f);
                        stuckTime[i] = 0.0f;
                    }
                }

                // Always step so lane/yield smoothing continues even when stopped
                v->step(dt, allowMove);

                // Hard safety: keep vehicles on driveable tiles
                if (!cityMap->isDriveableWorld(v->getPosition())) {
                    v->setPosition(cityMap->clampToDriveable(v->getPosition()));
                }
            }
        }
    }

    void Draw() {
        if (ambulance) ambulance->draw();
        for (auto v : normalTraffic) v->draw();
        for (const auto& l : lights) l.draw();

        if (!previewPath.empty() && !ambulanceMoving) {
            Color flashColor = ((int)(GetTime() * 5) % 2 == 0) ? RED : BLUE;
            for (size_t i = 0; i < previewPath.size() - 1; i++) {
                DrawLine3D(Vector3Add(previewPath[i], {0, 0.5f, 0}), Vector3Add(previewPath[i+1], {0, 0.5f, 0}), flashColor);
            }
        }
    }

}; // <--- THIS WAS MISSING (Closes the Simulation Class)

// =====================================================
// MAIN FUNCTION
// =====================================================
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int w = (int)(GetMonitorWidth(0) * 0.85f);
    int h = (int)(GetMonitorHeight(0) * 0.85f);
    if (w < 1000) w = 1000;
    if (h < 700) h = 700;
    InitWindow(w, h, "Smart City - Synchronized Lane Simulation");
    SetWindowMinSize(900, 600);
    SetTargetFPS(60);
    
    CityMap city; 
    Simulation sim(&city);

    Camera3D cam = {0};
    cam.target = {20, 0, 20}; 
    cam.up = {0, 1, 0};
    cam.fovy = 45;
    cam.projection = CAMERA_PERSPECTIVE;

    float camAngle = 45.0f;
    float camRadius = 80.0f;
    float camHeight = 40.0f;

    while (!WindowShouldClose()) {   
        float rotationSpeed = 60.0f * GetFrameTime();
        float zoomSpeed = 40.0f * GetFrameTime();

        if (IsKeyDown(KEY_A)) camAngle -= rotationSpeed;
        if (IsKeyDown(KEY_D)) camAngle += rotationSpeed;
        if (IsKeyDown(KEY_W)) camRadius -= zoomSpeed; 
        if (IsKeyDown(KEY_S)) camRadius += zoomSpeed;
        
        camRadius = Clamp(camRadius, 15.0f, 150.0f);

        cam.position.x = cam.target.x + cosf(camAngle * DEG2RAD) * camRadius;
        cam.position.z = cam.target.z + sinf(camAngle * DEG2RAD) * camRadius;
        cam.position.y = camHeight;

        sim.Update(cam);

        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode3D(cam);
                city.draw(); 
                sim.Draw();
            EndMode3D();
            DrawText("A/D: Rotate | W/S: Zoom | L-Click: Set Path | G: Go", 10, 10, 20, DARKGREEN);
        EndDrawing();
    } 
    CloseWindow();
    return 0;
}