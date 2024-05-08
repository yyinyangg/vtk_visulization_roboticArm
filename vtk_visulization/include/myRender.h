#pragma once
#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2);     // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);    // Build with vtkTextActor
#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPlaneSource.h> // ���ƽ��Դͷ�ļ�
#include <vtkPolyDataMapper.h> // ��Ӷ��������ӳ��ͷ�ļ�
#include <vtkActor.h> // �����Աͷ�ļ�
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkTextProperty.h>
#include <vtkAxisActor2D.h>
#include <vtkCaptionActor2D.h>
#include <vector>

class MyRender {
private:
	int num_of_joints;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<vtkPlaneSource> plane;
    vtkSmartPointer<vtkPolyDataMapper> planeMapper;
    vtkSmartPointer<vtkActor> planeActor;

    std::vector<vtkSmartPointer<vtkTransform>> transforms;
    std::vector<vtkSmartPointer<vtkAxesActor>> axes;
    std::vector<vtkSmartPointer<vtkLineSource>> lines;
public:
    MyRender(int);
    void init_jointPositions();
    void connet_joints();
    void show();

    //ʹ��update_position�����������б�transforms�ж�Ӧ�ڸ����ؽڵı任����
    void update_position();
};
