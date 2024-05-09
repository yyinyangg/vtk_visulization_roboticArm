#include "../include/myRender.h"

MyRender::MyRender(int num = 7):num_of_joints(num)
{
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // 创建一个灰色平面
    plane = vtkSmartPointer<vtkPlaneSource>::New();
    plane->SetResolution(10, 10);
    plane->SetOrigin(-10, -10, 0);

    // 设置平面的两个点，确定平面的大小和方向
    plane->SetPoint1(-20, 30, 0);
    plane->SetPoint2(20, -20, 0);

    plane->Update();
  
    vtkSmartPointer<vtkExtractEdges> edges = vtkSmartPointer<vtkExtractEdges>::New();
    edges->SetInputConnection(plane->GetOutputPort());


    planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputConnection(edges->GetOutputPort());

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

vtkSmartPointer<vtkLineSource> MyRender::create_line(int index) {
    vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
    double p2[3];
    if (index - 1 < 0) {
        double p1[3] = { 0 };
        line->SetPoint1(p1); // S_i
    }
    else {
        double p1[3];
        vtkSmartPointer<vtkLinearTransform> transform1 = axes[index - 1]->GetUserTransform();
        transform1->TransformPoint(axes[index - 1]->GetOrigin(), p1);
        line->SetPoint1(p1); // S_i
    }
    vtkSmartPointer<vtkLinearTransform> transform2 = axes[index]->GetUserTransform();
    transform2->TransformPoint(axes[index]->GetOrigin(), p2);

    line->SetPoint2(p2); //S_i+1
    return line;
}

void MyRender::connet_joints() 
{
    for (int i = 0; i < num_of_joints; i++) {
        vtkSmartPointer<vtkLineSource> line = create_line(i);
        
        // 创建管道滤波器
        vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter->SetInputConnection(line->GetOutputPort());
        if (i==0)tubeFilter->SetRadius(2.5);
        else tubeFilter->SetRadius(0.5);
        tubeFilter->SetNumberOfSides(20);
        tubeFilter->CappingOn();
        tubeFilter->SidesShareVerticesOn();
        tubeFilter->Update();
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

    // 设置相机属性来调整初始的视角
    renderer->GetActiveCamera()->SetPosition(60, 60, 120);
    renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->SetViewUp(-0.08, -0.08, 1);
    //renderer->ResetCamera(); // 自动调整相机位置和焦点以适应场景
    
    // 渲染并显示场景
    renderWindow->Render();
    renderWindowInteractor->Start();

}

void MyRender::update_DHTable(std::vector<double> angle_inDegree) {
    for (int i = 0;i < num_of_joints;i++) {
        table_ptr->table[i].rotZ += angle_inDegree[i];
    }
    table_ptr->cal_transforms();
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