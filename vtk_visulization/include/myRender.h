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
#include <vtkPlaneSource.h> // 添加平面源头文件
#include <vtkPolyDataMapper.h> // 添加多边形数据映射头文件
#include <vtkActor.h> // 添加演员头文件
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkTextProperty.h>
#include <vtkAxisActor2D.h>
#include <vtkCaptionActor2D.h>
#include <vtkTubeFilter.h>
#include <vtkWarpScalar.h>
#include <vtkCamera.h>
#include <vtkExtractEdges.h>
#include <vector>
#include <memory>

#define PI 3.141592653589793238462643383279502884      
#define PI_2 1.570796326794896619231321691639751442    
#define PI_4 0.785398163397448309615660845819875721    
#define deg2rad 0.017453292519943295769236907684886127 // PI/180
#define rad2deg 57.29577951308232087679815481410517033 // 180/PI

class MDH_Table;
struct MDHPara;
class MyRender {
private:
	int num_of_joints;

    std::unique_ptr<MDH_Table> table_ptr;
    vtkSmartPointer<vtkAxesActor> origin;
    std::vector<vtkSmartPointer<vtkAxesActor>> axes;
    std::vector<vtkSmartPointer<vtkTubeFilter>> parts;
public:
    vtkSmartPointer<vtkPlaneSource> plane;
    vtkSmartPointer<vtkPolyDataMapper> planeMapper;
    vtkSmartPointer<vtkActor> planeActor;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    MyRender(int);
    void init_axes();
    void updata_axes();
    void connet_joints();
    void show();
    vtkSmartPointer<vtkLineSource> create_line(int index);

    //使用 void update_DHTable()函数来更新MDH参数列表
    void update_DHTable( std::vector<double> angle_inDegree);
};



struct MDHPara
{
    float rotX, dipX, rotZ, dispZ;
    MDHPara(float rotX, float dipX, float rotZ, float dispZ) :rotX(rotX), dipX(dipX), rotZ(rotZ), dispZ(dispZ) {}
};

class MDH_Table
{
private:
    int num_joints;
    std::vector<MDHPara> table;
    std::vector<vtkSmartPointer<vtkTransform>> rel_transforms, abs_transforms;
public:
    friend MyRender;
    MDH_Table(int);
    std::vector<MDHPara> init();
    void cal_transforms();
    std::vector<vtkSmartPointer<vtkTransform>>& get_absTra();
};
