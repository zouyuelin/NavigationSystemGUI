#include "vtkrender.h"

using namespace std;

void vtkRender::vtkplot(QVTKWidget *vtkwidget)
{
    vtkSmartPointer<vtkAxesActor> actor2 = vtkSmartPointer<vtkAxesActor>::New(); //三维坐标系
        actor2->SetPosition(0, 0, 0);
        actor2->SetTotalLength(100, 100, 100);
        actor2->SetShaftType(0);
        actor2->SetAxisLabels(0);

    vtkSmartPointer<vtkActor> Actor = vtkSmartPointer<vtkActor>::New();

    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();									//----���û�����(���ƶ���ָ��)
        ren->AddActor(Actor);
        ren->AddActor(actor2);

    //renWin = vtkwidget->GetRenderWindow();//vtkSmartPointer<vtkRenderWindow>::New();      				//----���û��ƴ���
        vtkwidget->GetRenderWindow()->AddRenderer(ren);

    //iren = vtkwidget->GetInteractor();//vtkSmartPointer<vtkRenderWindowInteractor>::New();	//----���û��ƽ����������ڵ�
        vtkwidget->GetRenderWindow()->GetInteractor()->SetRenderWindow(vtkwidget->GetRenderWindow());

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();	//----���������
        vtkwidget->GetInteractor()->SetInteractorStyle(style);


    vtkSmartPointer<vtkDICOMImageReader> Reader = vtkSmartPointer<vtkDICOMImageReader>::New();
        std::string str = "/home/zyl/qtWorkPath/TestVTK/PA0/ST0/SE2";
        Reader->SetDataByteOrderToLittleEndian();
        Reader->SetDataScalarTypeToUnsignedChar();
        Reader->SetDataScalarType(VTK_UNSIGNED_CHAR);  //������ת��Ϊunsigned char��,vtkֻ֧��unsigned char�ͺ�short�������
        Reader->SetFileDimensionality(3);
        Reader->SetFileNameSliceSpacing(1);
        //Reader->SetDataExtent( 0, 511, 0, 511, 300, 515 ); //ͼ���СΪ512*512
        Reader->SetDirectoryName(str.c_str());
        Reader->SetDataSpacing( 0.3 ,0.3 ,1  );//��������֮���֡ͼ��֮��ļ��
        Reader->SetDataOrigin( 0.0, 0.0, 0.0 );
        Reader->Update();

        qDebug()<<"the width is :"<<QString::number(Reader->GetWidth())<<endl;
        qDebug()<<"the height is :"<<QString::number(Reader->GetHeight());

        vtkSmartPointer<vtkMarchingCubes> skinExtractor = vtkSmartPointer<vtkMarchingCubes>::New();       	//----����һ��Marching Cubes �㷨�Ķ���
            skinExtractor->SetInputConnection(Reader->GetOutputPort());     //�������͵�ת��
            skinExtractor->SetNumberOfContours(1);
            skinExtractor->SetValue(1,200);

        vtkSmartPointer<vtkDecimatePro> deci=vtkSmartPointer<vtkDecimatePro>::New(); 						//----�������ݶ�ȡ�㣬���������������ٽ���
            deci->SetTargetReduction(0.3);
            deci->SetInputConnection(skinExtractor->GetOutputPort());

        vtkSmartPointer<vtkSmoothPolyDataFilter> smooth=vtkSmartPointer<vtkSmoothPolyDataFilter>::New();  	//----ʹͼ����ӹ⻬
            smooth->SetInputConnection(deci->GetOutputPort());
            smooth->SetNumberOfIterations(200) ;

        vtkSmartPointer<vtkPolyDataNormals> skinNormals =vtkSmartPointer<vtkPolyDataNormals>::New();		//----����
            skinNormals->SetInputConnection(smooth->GetOutputPort());
            skinNormals->SetFeatureAngle(60.0);
            //skinNormals->SetInputConnection(WindowedSincFilter1->GetOutputPort());
            skinNormals->SetComputePointNormals(1);                            //�����㷨�����ļ���
            skinNormals->SetComputeCellNormals(0);                             //�رյ�Ԫ�������ļ���
            skinNormals->SetAutoOrientNormals(1);                              //�Զ������������ķ���
            skinNormals->SplittingOff();                                       //�رնԱ�Ե��
            skinNormals->Update();

        vtkSmartPointer<vtkStripper> stripper= vtkSmartPointer<vtkStripper>::New();   						//----����������������
            stripper->SetInputConnection(skinNormals->GetOutputPort());

        vtkSmartPointer<vtkPolyDataMapper> Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            Mapper->SetInputConnection(stripper->GetOutputPort());
            Mapper->ScalarVisibilityOff();

        vtkSmartPointer<vtkCamera> aCamera = vtkSmartPointer<vtkCamera>::New();   							//----���������
            aCamera->SetViewUp ( 0, 0, -1 );
            aCamera->SetPosition ( 0, 1, 0 );
            aCamera->SetFocalPoint( 0, 0, 0 );
            aCamera->ComputeViewPlaneNormal();

        vtkSmartPointer<vtkJPEGReader> m_pJPEGReader=vtkSmartPointer<vtkJPEGReader>::New();					//----��������
            m_pJPEGReader->SetFileName("/home/zyl/qtWorkPath/TestVTK/testvtk_1/texture.jpg");

        vtkSmartPointer<vtkTexture> m_pTexture=vtkSmartPointer<vtkTexture>::New();
            m_pTexture->SetInputConnection(m_pJPEGReader->GetOutputPort());
            m_pTexture->InterpolateOn();

        vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
            property->SetDiffuseColor(1, 0.49, 0.25);                        //����Ƥ����ɫ������//(1, .49, .25)
            property->SetDiffuse(0.7);
            property->SetSpecular(0.3);                                      //���÷�����
            property->SetSpecularPower(20);                                  //���÷����ǿ��
            property->SetOpacity(0.3);                                       //����͸����

            Actor->SetMapper( Mapper );
            Actor->SetTexture(m_pTexture);								//���Ƥ���������ݵ�����
            Actor->SetProperty(property);

        ren->SetActiveCamera(aCamera); 									//������Ⱦ�������
        ren->ResetCamera();

    ren->SetBackground(0.1,0.2,0.4);


    //vtkwidget->GetRenderWindow()->Render();
    //vtkwidget->GetInteractor()->Initialize();
    //vtkwidget->GetInteractor()->Start();
    vtkwidget->GetInteractor()->Start();
}
