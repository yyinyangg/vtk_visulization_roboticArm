#include "../include/myRender.h"
#include <chrono> 
#include <thread> 

int main() {
    MyRender myRender = MyRender(6);

    std::vector<std::vector<double>> frames;

    //≥ı ºŒª÷√
    frames.push_back(std::vector<double>({ 0, 0, 0, 0, 0, 0 }));
    //J2
    frames.push_back(std::vector<double>({ -8.72, 16.68, 18.746, -96.139, 4.99, 139.451 }));
    //J3
    frames.push_back(std::vector<double>({ -45.896, 0, 0, 0, 60, 90 }));
    
    for (auto f : frames) {
        myRender.update_DHTable(f);
        myRender.show();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // —”≥Ÿ100∫¡√Î
        myRender.renderer->RemoveAllViewProps();
    }
    
    return 0;
}


