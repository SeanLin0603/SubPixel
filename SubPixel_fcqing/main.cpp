#include "pch.h"
#include "edgeTest.h"
#include "WriteFile.h"
#include <vector>
using namespace std;
using namespace cv;

int main()
{
	Mat srcImage, grayImage, dstImage;
	srcImage = imread("img1.jpg");
	if (srcImage.empty())
	{
		cout << "load error!" << endl;
		return -1;
	}

	//parameters setting
	double * x;          /* x[n] y[n] coordinates of result contour point n */
	double *	y;
	int * curve_limits;  /* limits of the curves in the x[] and y[] */
	int N, M;         /* result: N contour points, forming M curves */
	double S = 1.5; /* default sigma=0 */
	double H = 4.2; /* default th_h=0  */
	double L = 0.81; /* default th_l=0  */
	double W = 1; /* default W=1.3   */
	char * pdf_out = _strdup("output.pdf");  /*pdf filename*/
	char * txt_out = _strdup("output.txt");

	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
	dstImage = grayImage;
	const int iHeight = dstImage.rows;
	const int iWidth = dstImage.cols;
	uchar* pSrc = grayImage.data;//new uchar[iHeight*iWidth];
	uchar* pDst = dstImage.data;

	//imshow("input image", grayImage);
	devernay(&x, &y, &N, &curve_limits, &M, pSrc, pDst, iWidth, iHeight, S, H, L);

	int scale = 11;
	Mat drawImage = srcImage.clone();
	Size size(iWidth*scale, iHeight*scale);
	resize(srcImage, drawImage, size, 0.0, 0.0, INTER_NEAREST);
	
#pragma region drawImage
	vector<vector<Point2f>> data;
	for (int i = 0; i < M; i++)
	{
		// M contours
		vector<Point2f> contour;
		for (int j = curve_limits[i]; j < curve_limits[i + 1]; j++)
		{
			// a contour
			Point2f point(x[j], y[j]);
			contour.push_back(point);
		}
		data.push_back(contour);
	}

	int maxIdx = -1, maxSize = -1;
	for (int i = 0; i < data.size(); i++)
	{
		int size = data[i].size();
		if (size > maxSize)
		{
			maxIdx = i;
			maxSize = size;
		}
	}

	for (int i = 0; i < maxSize; i++)
	{
		int x = (int)(data[maxIdx][i].x * scale + scale / 2);
		int y = (int)(data[maxIdx][i].y * scale + scale / 2);

		// BGR
		drawImage.at<Vec3b>(y, x)[0] = 0;
		drawImage.at<Vec3b>(y, x)[1] = 0;
		drawImage.at<Vec3b>(y, x)[2] = 255;
	}

	imwrite("output.png", drawImage);
#pragma endregion

	if (pdf_out != NULL) write_curves_pdf(x, y, curve_limits, M, pdf_out, iWidth, iHeight, W);
	if (txt_out != NULL) write_curves_txt(x, y, curve_limits, M, txt_out);

	//imshow("gaussion filtered image", dstImage);
	waitKey();
	//system("pause");
	return 0;
}
