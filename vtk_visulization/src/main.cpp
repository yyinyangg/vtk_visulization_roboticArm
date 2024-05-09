//#include "../include/myRender.h"
//
//int main() {
//    MyRender myRender = MyRender(6);
//    myRender.show();
//
//    return 0;
//}

#include <vtkSmartPointer.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTubeFilter.h>

int main() {
    // 创建一个圆柱体源
    vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
    cylinderSource->SetHeight(3.0); // 设置高度
    cylinderSource->SetRadius(1.0); // 设置半径
    cylinderSource->SetResolution(50); // 设置圆柱体的分辨率，决定了圆柱体表面的光滑程度

    // 创建一个管道滤波器，将圆柱体转换为管道
    vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(cylinderSource->GetOutputPort());
    tubeFilter->SetRadius(0.3); // 设置管道的半径

    // 创建一个映射器
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(tubeFilter->GetOutputPort());

    // 创建一个演员
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // 创建渲染器、渲染窗口和交互器
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // 将演员添加到渲染器中
    renderer->AddActor(actor);

    // 设置渲染器的背景色
    renderer->SetBackground(0.1, 0.2, 0.4);

    // 渲染并启动交互
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

