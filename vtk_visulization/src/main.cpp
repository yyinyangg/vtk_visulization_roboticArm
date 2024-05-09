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
    // ����һ��Բ����Դ
    vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
    cylinderSource->SetHeight(3.0); // ���ø߶�
    cylinderSource->SetRadius(1.0); // ���ð뾶
    cylinderSource->SetResolution(50); // ����Բ����ķֱ��ʣ�������Բ�������Ĺ⻬�̶�

    // ����һ���ܵ��˲�������Բ����ת��Ϊ�ܵ�
    vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(cylinderSource->GetOutputPort());
    tubeFilter->SetRadius(0.3); // ���ùܵ��İ뾶

    // ����һ��ӳ����
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(tubeFilter->GetOutputPort());

    // ����һ����Ա
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // ������Ⱦ������Ⱦ���ںͽ�����
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // ����Ա��ӵ���Ⱦ����
    renderer->AddActor(actor);

    // ������Ⱦ���ı���ɫ
    renderer->SetBackground(0.1, 0.2, 0.4);

    // ��Ⱦ����������
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

