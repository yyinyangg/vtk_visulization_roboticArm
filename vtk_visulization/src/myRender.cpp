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
    plane->SetResolution(100, 100);
    plane->SetOrigin(-10, 10, 0); // ����ƽ�����
    plane->SetPoint1(50, 50, 0);  // ����ƽ���ϵ�һ����
    plane->SetNormal(0, 0, 1);
    plane->Update();
    // ����һ��������
    vtkSmartPointer<vtkWarpScalar> warpScalar = vtkSmartPointer<vtkWarpScalar>::New();
    warpScalar->SetInputConnection(plane->GetOutputPort());


    planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputConnection(warpScalar->GetOutputPort());

    planeActor = vtkSmartPointer<vtkActor>::New();
    planeActor->SetMapper(planeMapper);
    planeActor->GetProperty()->SetColor(0.5, 0.5, 0.5); // ��ɫ

    //����MHD����������ʼ��
    table_ptr.reset(new MDH_Table(num_of_joints));
    init_axes();
    updata_axes();
}
void set_axe(const vtkSmartPointer<vtkAxesActor>& axe, 
    int lengthX , 
    int lengthY, 
    int lengthZ, 
    int width ) 
{
    axe->SetTotalLength(lengthX, lengthY, lengthZ);
    axe->GetXAxisTipProperty()->SetLineWidth(width);// �����ߵĿ��

    // ��ȡ�������ǩ���ԣ������������С
    float fontsize = 0.03;
    axe->GetXAxisCaptionActor2D()->SetWidth(fontsize);
    axe->GetXAxisCaptionActor2D()->SetHeight(fontsize);
    axe->GetYAxisCaptionActor2D()->SetWidth(fontsize);
    axe->GetYAxisCaptionActor2D()->SetHeight(fontsize);
    axe->GetZAxisCaptionActor2D()->SetWidth(fontsize);
    axe->GetZAxisCaptionActor2D()->SetHeight(fontsize);
}

void MyRender::init_axes()
{

    //������������ʾ��С
    int lengthX = 7, lengthY = 7, lengthZ = 7, width = 100;
    origin= vtkSmartPointer<vtkAxesActor>::New();
    set_axe(origin, lengthX , lengthY , lengthZ , width);
    origin->GetXAxisShaftProperty()->SetColor(0,0,0);
    origin->GetXAxisTipProperty()->SetColor(0, 0, 0);
    origin->GetYAxisShaftProperty()->SetColor(0, 0, 0);
    origin->GetYAxisTipProperty()->SetColor(0, 0, 0);
    origin->GetZAxisShaftProperty()->SetColor(0, 0, 0);
    origin->GetZAxisTipProperty()->SetColor(0, 0, 0);
    for (int i = 0; i < num_of_joints; i++) {
        vtkSmartPointer<vtkAxesActor> axe = vtkSmartPointer<vtkAxesActor>::New();
        set_axe(axe, lengthX, lengthY, lengthZ, width);
        axes.push_back(axe);
    }
}

void MyRender::updata_axes()
{
    std::vector<vtkSmartPointer<vtkTransform>> transforms = table_ptr->get_absTra();
    for (int i = 0;i < num_of_joints;i++)  axes[i]->SetUserTransform(transforms[i]);
}

void MyRender::connet_joints() 
{
    for (int i = 0; i < num_of_joints-1; i++) {
        vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
        line->SetPoint1(axes[i]->GetOrigin()); // S_i
        line->SetPoint2(axes[i+1]->GetOrigin()); //S_i+1
        
        // �����ܵ��˲���
        vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter->SetInputConnection(line->GetOutputPort());
        tubeFilter->SetRadius(10); // ���ùܵ��İ뾶
        parts.push_back(tubeFilter);
    }
}

void MyRender::show()
{
    connet_joints();
    renderer->AddActor(planeActor);
    renderer->AddActor(origin);
    for (auto axe : axes) {
        renderer->AddActor(axe); 
        //break;
    }
   
    for (auto p : parts) 
    {
        //ӳ����
        vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        lineMapper->SetInputConnection(p->GetOutputPort());
        //��Ա
        vtkSmartPointer<vtkActor> act = vtkSmartPointer<vtkActor>::New();
        act->SetMapper(lineMapper);
        // ���ùܵ���ɫΪ����ɫ
        vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
        tubeProperty->SetColor(0.5, 0.5, 1.0); // RGBֵ����������Ϊ����ɫ
        act->SetProperty(tubeProperty); 
        renderer->AddActor(act);
    }

    renderer->SetBackground(0.1, 0.2, 0.4);
    
    // ��Ⱦ����ʾ����
    renderWindow->Render();
    renderWindowInteractor->Start();

}

MDH_Table::MDH_Table(int num):num_joints(num)
{
    table = init();
    cal_transforms();
}

std::vector<MDHPara> MDH_Table::init()
{
    std::vector<MDHPara> table;
    table.push_back(MDHPara(0, 0, 0, 422));
    table.push_back(MDHPara(-0.5 * PI, 193.951, -0.5 * PI, 0));
    table.push_back(MDHPara(0, 681.292, -PI, 0.2913));
    table.push_back(MDHPara(0.5 * PI, -175.478, -PI, 744.788));
    table.push_back(MDHPara(0.5 * PI, 0, -PI, 0));
    table.push_back(MDHPara(0.5 * PI, 0, 0, 117.5));

    return table;
}

void MDH_Table::cal_transforms()
{
    float scale = 35;//��������Сscale��
    //���� S_i ����� S_i-1 ����Է���任
    //Ȼ�����ÿ�������ۼӵķ���任
    vtkSmartPointer<vtkTransform> abstr = vtkSmartPointer<vtkTransform>::New();
    //abstr->PostMultiply();
    for (auto ele : table)
    {   
        vtkSmartPointer<vtkTransform> tr = vtkSmartPointer<vtkTransform>::New();
        tr->RotateWXYZ(ele.rotX * rad2deg, 1, 0, 0);
        tr->Translate(ele.dipX / scale, 0, 0);
        
        tr->RotateWXYZ(ele.rotZ*rad2deg,0,0,1);
        tr->Translate(0, 0, ele.dispZ / scale);
        
        rel_transforms.push_back(tr);

        abstr->Concatenate(tr);
        vtkSmartPointer<vtkTransform> t_i = vtkSmartPointer<vtkTransform>::New();
        t_i->DeepCopy(abstr);
        abs_transforms.push_back(t_i);
    }
}
std::vector<vtkSmartPointer<vtkTransform>>& MDH_Table::get_absTra() {
    return abs_transforms;
}