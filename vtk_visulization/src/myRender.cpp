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

    // 创建一个灰色平面
    plane = vtkSmartPointer<vtkPlaneSource>::New();
    plane->SetResolution(100, 100);
    plane->SetOrigin(-10, 10, 0); // 设置平面起点
    plane->SetPoint1(50, 50, 0);  // 设置平面上的一个点
    plane->SetNormal(0, 0, 1);
    plane->Update();
    // 创建一个放样器
    vtkSmartPointer<vtkWarpScalar> warpScalar = vtkSmartPointer<vtkWarpScalar>::New();
    warpScalar->SetInputConnection(plane->GetOutputPort());


    planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputConnection(warpScalar->GetOutputPort());

    planeActor = vtkSmartPointer<vtkActor>::New();
    planeActor->SetMapper(planeMapper);
    planeActor->GetProperty()->SetColor(0.5, 0.5, 0.5); // 灰色

    //创建MHD参数表，并初始化
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
    axe->GetXAxisTipProperty()->SetLineWidth(width);// 设置线的宽度

    // 获取坐标轴标签属性，并设置字体大小
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

    //调整坐标轴显示大小
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
        
        // 创建管道滤波器
        vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter->SetInputConnection(line->GetOutputPort());
        tubeFilter->SetRadius(10); // 设置管道的半径
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
        //映射器
        vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        lineMapper->SetInputConnection(p->GetOutputPort());
        //演员
        vtkSmartPointer<vtkActor> act = vtkSmartPointer<vtkActor>::New();
        act->SetMapper(lineMapper);
        // 设置管道颜色为淡蓝色
        vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
        tubeProperty->SetColor(0.5, 0.5, 1.0); // RGB值，这里设置为淡蓝色
        act->SetProperty(tubeProperty); 
        renderer->AddActor(act);
    }

    renderer->SetBackground(0.1, 0.2, 0.4);
    
    // 渲染并显示场景
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
    float scale = 35;//将距离缩小scale倍
    //计算 S_i 相对于 S_i-1 的相对放射变换
    //然后计算每个坐标累加的仿射变换
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