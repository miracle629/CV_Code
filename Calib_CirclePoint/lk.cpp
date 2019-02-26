#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "cvut.h"
#include <cxcore.h>

int main( int argc, char** argv )
{
	FILE *fp;
	ofstream fout("caliberation_result.txt");
	float array1[81];
	float array2[81];
	float array3[9][9];
	float array4[9][9];
	float array5[1215];
	float array6[1215];

	double toparea=0;
	double maxarea=500;
	double minarea=100;
	int i=0;
	int j=0;
	int r=0;
	int s=0;
	int t=0;
	int p=0;
	int k=0;
	int q=0;

	CvScalar color1 = CV_RGB( 0, 255,0 );
	CvScalar color2 = CV_RGB( 0, 0,255 );

	CvBox2D box;
	CvSeq* contour = 0;
	IplImage * src;
	IplImage * dst;
	IplImage * src1;

	int m=201;
	char buf[10];
	for(k=1;k<=15;k++)
	{
    sprintf(buf,"%d.bmp",m);
	src = cvLoadImage(buf,CV_LOAD_IMAGE_GRAYSCALE);
	src1 = cvLoadImage(buf,1);
	m++;
	dst = cvCreateImage( cvGetSize(src), 8, 3 );
	
	CvMemStorage* storage = cvCreateMemStorage(0);
	
	cvThreshold( src, src,160, 255, CV_THRESH_BINARY );//��ֵ��
	cvFindContours( src, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );//��ȡ����
	cvZero( dst );//�������
	i=0;
	for( ; contour != 0; contour = contour->h_next )
	{
		
		double tmparea=fabs(cvContourArea(contour));//����������
		if(tmparea < minarea) 
		{
			cvSeqRemove(contour,0); //ɾ�����С���趨ֵ������
			continue;
		}
		if(tmparea > maxarea) 
		{
			cvSeqRemove(contour,0); //ɾ����������趨ֵ������
			continue;
		}
		CvRect aRect = cvBoundingRect( contour, 0 );//�����С��Ӿ���
		if(aRect.height<8)
		{
			cvSeqRemove(contour,0); //ɾ���߶�С���趨ֵ������
			continue;
		}
		if ((aRect.width/aRect.height)>2)
		{
			cvSeqRemove(contour,0); //ɾ����߱��������趨ֵ������
			continue;
		}
		box=cvMinAreaRect2(contour,0);//��Ͽ���ת����С��Ӿ���
		float x=box.center.x;//��������
		float y=box.center.y;
		
		array1[i]=x;
		array2[i]=y;
		i++;	 
		cvDrawContours( dst, contour, color1, color2, 0, 1, 8 );//��������
	}
	
	/*�����������ս��Ϊ���������������У�����������������*/
	for(r=0;r<9;r++)
	{
		for(s=0;s<9;s++)
		{
			array3[r][s]=array1[9*r+s];
			array4[r][s]=array2[9*r+s];
		}
	}
	
	for(r=0;r<9;r++)
	{
		for (s=0;s<8;s++)         
		{
			for (t=0;t<8-s; t++) 
			{
				if (array3[r][t]>array3[r][t+1])            
				{
					float temp3=array3[r][t];
					array3[r][t]=array3[r][t+1];
					array3[r][t+1]=temp3;
					float temp4=array4[r][t];
					array4[r][t]=array4[r][t+1];
					array4[r][t+1]=temp4;
				}
            }
        }
    }

	for(r=0;r<4;r++)
	{
		for(s=0;s<9;s++)
		{
			float temp5=array3[r][s];
			array3[r][s]=array3[8-r][s];
			array3[8-r][s]=temp5;
			float temp6=array4[r][s];
			array4[r][s]=array4[8-r][s];
			array4[8-r][s]=temp6;
		}
	}

	p=0;
	for(r=0;r<9;r++)
	{
		for(s=0;s<9;s++)
		{
			array1[p]=array3[r][s];
			array2[p]=array4[r][s];
			p++;
		}
	}

	for(i=0;i<81;i++)
	{
		array5[q]=array1[i];
		array6[q]=array2[i];
		q++;
	}
	
	printf("\n��%d��ͼԲ������\n",k);

	for(i=0;i<81;i++)
	{
		
		printf("%f ",array1[i]);
		printf("%f\n",array2[i]);
	}

	cvNamedWindow( "Image", 1 );
	cvNamedWindow( "Circles", 1 );
	cvShowImage( "Image", src1 );
	cvShowImage( "Circles", dst );
	cvWaitKey(0);
	}

	cvDestroyWindow( "Image" );
	cvReleaseImage(&src1);
	cvDestroyWindow( "Circles" );
	cvReleaseImage(&dst);

	printf("\nData is OK\n");

	cvWaitKey(0);

	fp=fopen("data.txt","w+");
	for(i=0;i<1215;i++)
	{
		fprintf(fp,"%f ",array5[i]);
		fprintf(fp,"%f\n",array6[i]);
	}
	fclose(fp);

	cvWaitKey(0);

	int image_count=15;

	CvSize board_size = cvSize(9,9);//ÿ����Ƭ9*9��Բ
	CvSize image_size = cvSize(800,600);//ͼ��ߴ�800*600����

	double square_size[2]={18.4,18.4};//ʵ��Բ�ľ���18.4mm

	Matrix<double> object_points(1,board_size.width*board_size.height*image_count,3);//��άʵ������
	Matrix<double> image_points(1,image_count*board_size.width*board_size.height,2); //��άͼ������
	Matrix<int> point_counts(1,image_count,1); // ÿ��ͼ���нǵ������
	Matrix<double> intrinsic_matrix(3,3,1); //������ڲ�������3*3
	Matrix<double> distortion_coeffs(1,4,1); //�������4������ϵ����k1,k2,p1,p2
	Matrix<double> rotation_vectors(1,image_count,3); //ÿ��ͼ�����ת����
	Matrix<double> translation_vectors(1,image_count,3);//ÿ��ͼ���ƽ������

	
	for (t=0;t<image_count;t++) 
	{
		for (i=0;i<board_size.height;i++) 
		{
			for (j=0;j<board_size.width;j++) 
			{
				/* ���趨��������������ϵ��z=0��ƽ���� */
				object_points(0,t*board_size.height*board_size.width+i*board_size.width+j,0) = i*square_size[0];
				object_points(0,t*board_size.height*board_size.width+i*board_size.width+j,1) = j*square_size[1];
				object_points(0,t*board_size.height*board_size.width+i*board_size.width+j,2) = 0;
			}
		}
	}

	for (i=0;i<image_count*board_size.width*board_size.height;i++) 
	{
		image_points(0,i,0) = array5[i];
		image_points(0,i,1) = array6[i];
	}

	for (i=0;i<image_count;i++)
	{
		point_counts(0,i) = board_size.width*board_size.height;
	}
	
	cvCalibrateCamera2(object_points.cvmat,
					   image_points.cvmat,
                       point_counts.cvmat,
					   image_size,
                       intrinsic_matrix.cvmat,
					   distortion_coeffs.cvmat,
                       rotation_vectors.cvmat,
					   translation_vectors.cvmat,
					   0);
	printf("\n��ɱ궨\n");
	printf("\n�ڲξ���:\n");
	cout<<intrinsic_matrix<<'\n';
	fout<<intrinsic_matrix<<'\n';
	printf("�������:\n");
	cout<<distortion_coeffs<<'\n';
	fout<<distortion_coeffs<<'\n';
	cvWaitKey(0);

	return 0;
}