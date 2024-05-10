#include "../include/myRender.h"


int main() {
    MyRender myRender = MyRender(6);

    
    std::vector<std::vector<double>> frames;

    //≥ı ºŒª÷√
    //frames.push_back(std::vector<double>({ 0, 0, 0, 0, 0, 0 }));
    //J2
    //frames.push_back(std::vector<double>({ -8.72, 16.68, 18.746, -96.139, 4.99, 139.451 }));
    //J3
    //frames.push_back(std::vector<double>({ -45.896, 0, 0, 0, 60, 90 }));
    int num_frames = 100;
    std::vector<double> J2({ -8.72, 16.68, 18.746, -96.139, 4.99, 139.451 });
    std::vector<double> J3({ -45.896, 0, 0, 0, 60, 90 });
    std::vector<double> step_size;
    for (int i = 0; i < J2.size(); i++) 
        step_size.push_back((J3[i] - J2[i]) / num_frames);

    for (int i = 0; i < num_frames; i++) 
        frames.push_back(std::vector<double>(
            { J2[0]+i*step_size[0], 
            J2[1]+i * step_size[0], 
            J2[2]+i * step_size[0], 
            J2[3]+i * step_size[0], 
            J2[4]+ i * step_size[0],
            J2[5]+ i * step_size[0]}));

    for (auto f : frames) {
        myRender.update_DHTable(f);
        myRender.show();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // —”≥Ÿ100∫¡√Î
    }
    myRender.renderWindowInteractor->Start();
    return 0;
}

