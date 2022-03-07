#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;


void crop(Mat img, vector<Rect> boundRect, int i); 
void getContours(Mat img_in, Mat img_out);
Mat procesare(Mat img_in, Mat img_out);


void crop(Mat img, vector<Rect> boundRect, int i)
{
	Rect roi(boundRect[i].tl(), boundRect[i].br());
	Mat cropped_img = img(roi);
	imshow("Cropped image", cropped_img);
	imwrite("Images/Plates/" + to_string(i) + ".jpg", cropped_img);
	cout << "\t" + to_string(i) + ".jpg" << endl;
}


void getContours(Mat img_in, Mat img_out)
{
	std::vector<std::vector<cv::Point> > contururi;
	vector<Vec4i> hierarchy;	//este un vector, dar are 4 int inauntru
	findContours(img_in, contururi, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	std::vector<std::vector<cv::Point> > Colturi(contururi.size());
	std::vector<Rect> boundRect(contururi.size());		//un vector in care stocam coordonatele tuturor contururilor
	//drawContours(img_out, contururi, -1, Scalar(255, 255, 0), 2);   //---pentru toate contururile
	
	for (int i = 0; i < contururi.size(); i++)
	{
		int area = contourArea(contururi[i]);
		if (area > 3000 )		//ignoram contururile foarte mici, fiindca acestea nu au cum sa fie placute
		{
			//drawContours(img_out, contours, -1, Scalar(255, 255, 0), 2);		--afisam contururile filtrate cu aria mai mare de 3000
			float perimetru = arcLength(contururi[i], true);
			approxPolyDP(contururi[i], Colturi[i], 0.02 * perimetru, true);		//extragem colturile din contururi
			//drawContours(img_out, Colturi, i, Scalar(0, 0, 255), 2);
			//cout << "Colturi: "<< conPoly[i].size() << endl;
			
			int obiect = (int)Colturi[i].size();	
			
			if (obiect == 4)
			{
				//cout << area << endl;
				cout << "Perimetru:" << perimetru << endl;
				//cv::drawContours(img_out, Colturi, i, Scalar(255, 0, 255), 2);
				boundRect[i] = boundingRect(Colturi[i]);	//obtinem coordonatele	
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				cout << "Asp ratio: " << aspRatio << endl;
				if (aspRatio < 0.93 || aspRatio > 1.1)		//conditie ca obiectul sa nu fie patrat, sa fie doar dreptunghi
				{
					rectangle(img_out, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
					crop(img_out, boundRect, i);
				}
			}
			
		}
	}
	
}


Mat procesare(Mat img_in)
{
	Mat imgGray, imgBlur, imgCanny, imgDil;

	cvtColor(img_in, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(img_in, imgBlur, Size(3, 3), 3, 0);  //Folosim Gaussian Blur pentru a elimina zgomotele de frecvente inalte
	Canny(imgBlur, imgCanny, 100, 100);	//Pentru edge detection
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, element);
	//imshow("Image Canny", imgCanny);
	//imshow("Dilate image", imgDil);
	return imgDil;
}



void main(int argc, char* argv[])
{
	//Citirea imaginii
	String filename = "Images/Car.jpg";
	Mat initimg = cv::imread(filename);
	imshow("Original", initimg);

	//Procesare
	Mat img = procesare(initimg);

	Mat contur = initimg;
	//Aplicam functia de contur pentru imaginea initiala
	getContours(img, contur);

	imshow("Output", contur);

	cv::waitKey(0);

	destroyAllWindows();

}

