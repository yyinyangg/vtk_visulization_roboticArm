#include "../include/myRender.h"
#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
MyRender::MyRender(int num = 7):num_of_joints(num)
{
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // ����һ����ɫƽ��
    plane = vtkSmartPointer<vtkPlaneSource>::New();
    plane->SetOrigin(-10, 10, 0); // ����ƽ�����
    plane->SetPoint1(50, 50, 0);  // ����ƽ���ϵ�һ����
    plane->SetNormal(0, 0, 1);
    plane->Update();

    planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputConnection(plane->GetOutputPort());

    planeActor = vtkSmartPointer<vtkActor>::New();
    planeActor->SetMapper(planeMapper);
    planeActor->GetProperty()->SetColor(0.5, 0.5, 0.5); // ��ɫ
    init_jointPositions();
}

void MyRender::init_jointPositions() 
{
    for (int i = 0; i < num_of_joints; i++) {
        vtkSmartPointer<vtkAxesActor> axe = vtkSmartPointer<vtkAxesActor>::New();
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Translate(10*i, 10*i, 0);
        transform->RotateZ(20 * i);
        axe->SetUserTransform(transform);

        //������������ʾ��С
        int lengthX = 10, lengthY = 10, lengthZ = 10, width = 15;
        axe->SetTotalLength(lengthX, lengthY, lengthZ);
        axe->GetXAxisTipProperty()->SetLineWidth(width);// �����ߵĿ��
        // ��ȡ�������ǩ���ԣ������������С
        float fontsize = 0.05;
        axe->GetXAxisCaptionActor2D()->SetWidth(fontsize);
        axe->GetXAxisCaptionActor2D()->SetHeight(fontsize);
        axe->GetYAxisCaptionActor2D()->SetWidth(fontsize);
        axe->GetYAxisCaptionActor2D()->SetHeight(fontsize);
        axe->GetZAxisCaptionActor2D()->SetWidth(fontsize);
        axe->GetZAxisCaptionActor2D()->SetHeight(fontsize);
       
        axes.push_back(axe);
    }
}
void MyRender::connet_joints() 
{
    for (int i = 0; i < num_of_joints-1; i++) {
        vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
        line->SetPoint1(axes[i]->GetOrigin()); // ԭ��
        line->SetPoint2(axes[i+1]->GetOrigin()); // ��ǰ�ο������ԭ��
        lines.push_back(line);
    }
}

void MyRender::show()
{
    renderer->AddActor(planeActor);
    connet_joints();
    for (auto axe : axes) renderer->AddActor(axe);
   
    for (auto line : lines) 
    {
        vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        lineMapper->SetInputConnection(line->GetOutputPort());
        vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();
        lineActor->SetMapper(lineMapper);
        lineActor->GetProperty()->SetColor(0, 0, 0); // ��ɫ
        renderer->AddActor(lineActor);
    }

    renderer->SetBackground(0.1, 0.2, 0.4);
    
    // ��Ⱦ����ʾ����
    renderWindow->Render();
    renderWindowInteractor->Start();

}