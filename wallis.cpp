//影像wallis filter 的opencv实现
Mat wallis(cv::Mat image)
{

	int n = image.cols;
	int m = image.rows;

	// Block dimension in i and j
	int dim_n = 100, dim_m = 100;

	const int N_Block = n / dim_n;
	const int M_Block = m / dim_m;
	int resto_n = n%dim_n;
	int resto_m = m%dim_m;

	//alloco due array lunghi N_Block e M_Block
	int *dimension_x = new int[N_Block];
	memset(dimension_x, 0, sizeof(dimension_x));
	int *dimension_y = new int[M_Block];
	memset(dimension_y,0,sizeof(dimension_y));
	//int dimension_x[N_Block];
	//int dimension_y[M_Block];

	dim_n = dim_n + resto_n / N_Block;
	dim_m = dim_m + resto_m / M_Block;
	resto_n = n%dim_n;
	resto_m = m%dim_m;

	int i;
	for (i = 0; i < N_Block; i++)
	{
		if (resto_n>0)
		{
			dimension_x[i] = dim_n + 1;
			resto_n--;
		}
		else
		{
			dimension_x[i] = dim_n;
		}
	}


	for (i = 0; i < M_Block; i++)
	{
		if (resto_m>0)
		{
			dimension_y[i] = dim_m + 1;
			resto_m--;
		}
		else
		{
			dimension_y[i] = dim_m;
		}
	}

	// c is the CONTRAST expansion constant [0.7-1.0]
	// to reduce the enhancement of noise c should be reduced
	// it has a much stronger influence on noise than sf
	// lower values produce very little contrast and detail
	// values closer to 1 produce a highly contrasted image with greater detail 
	double c = 0.8;

	// sf is the target value of the LOCAL STANDARD DEVIATION in a i,j window [50.0-80.0]
	// the value of sf should decrease with decreasing window dimensions(i,j) to avoid increase of noise
	// it decides the contrast of image; higher values result in a greater contrast stretch
	// producing higher local contrast and greater detail 
	double sf = 80.0;

	// b is the BRIGHTNESS forcing constant [0.5-1.0]
	// to keep primary image gray mean b has to be small
	// 0 will keep the original pixel values
	// 1 will generate an output image equal to the wallis filter specified
	double b = 0.9;

	// mf is the target value of the LOCAL MEAN in a i,j window [127.0-140.0]
	// an higher value wil brighten the image
	double mf = 127.0;

	int px = 0, py = 0;

	//ricorda che devi invertire M_Block e N_Block perchè l'immagine è ruotata
	cv::Mat Coeff_R0 = cv::Mat::zeros(M_Block, N_Block, CV_64F);
	//(N_Block, M_Block, CV_64F);
	cv::Mat Coeff_R1 = cv::Mat::zeros(M_Block, N_Block, CV_64F);
	// computing mean and standard deviation in every (dim_n*dim_m) window
	for (int i = 0; i<N_Block; i++)
	{
		py = 0;
		for (int j = 0; j<M_Block; j++)
		{
			cv::Mat block = image(cv::Rect(px, py, dimension_x[i], dimension_y[j]));
			cv::Scalar mean, stDev;
			cv::meanStdDev(block, mean, stDev);

			py += dimension_y[j];

			double r1 = c*sf / (c*stDev.val[0] + (1 - c)*sf);				//Baltsavias
			//double r1 = c*stDev.val[0]/(c*stDev.val[0] + (sf/c));		//Fraser	
			//double r1 = c*sf/(c*stDev.val[0] + (sf/c));				//Xiao				
			double r0 = b*mf + (1 - b - r1)*mean.val[0];

			Coeff_R1.at<double>(j, i) = r1;
			Coeff_R0.at<double>(j, i) = r0;
		}
		px += dimension_x[i];
	}

	cv::resize(Coeff_R1, Coeff_R1, image.size(), cv::INTER_LINEAR);
	cv::resize(Coeff_R0, Coeff_R0, image.size(), cv::INTER_LINEAR);

	image.convertTo(image, CV_64F);

	cv::Mat Filtered_image = cv::Mat::zeros(cv::Size(N_Block, M_Block), CV_64F);
	cv::multiply(image, Coeff_R1, Filtered_image);
	cv::add(Filtered_image, Coeff_R0, Filtered_image);

	double minVal, maxVal;
	minMaxLoc(Filtered_image, &minVal, &maxVal);
	Filtered_image.convertTo(Filtered_image, CV_8UC1, 255 / (maxVal - minVal), -minVal * 255 / (maxVal - minVal));
	minMaxLoc(image, &minVal, &maxVal);
	image.convertTo(image, CV_8UC1, 255 / (maxVal - minVal), -minVal * 255 / (maxVal - minVal));

	return Filtered_image;
}

int ImageStretchByHistogram(IplImage *src1, IplImage *dst1)
/*************************************************
Function:      通过直方图变换进行图像增强，将图像灰度的域值拉伸到0-255
src1:               单通道灰度图像
dst1:              同样大小的单通道灰度图像
*************************************************/
{
    assert(src1->width == dst1->width);
    double p[256], p1[256], num[256];

    memset(p, 0, sizeof(p));
    memset(p1, 0, sizeof(p1));
    memset(num, 0, sizeof(num));
    int height = src1->height;
    int width = src1->width;
    long wMulh = height * width;

	    //statistics  
    for (int x = 0; x<src1->width; x++)
    {
        for (int y = 0; y<src1->height; y++){
            uchar v = ((uchar*)(src1->imageData + src1->widthStep*y))[x];
                num[v]++;

		}
    }
    //calculate probability  
    for (int i = 0; i<256; i++)
    {
       p[i] = num[i] / wMulh;
    }

    //p1[i]=sum(p[j]);  j<=i;  
    for (int i = 0; i<256; i++)
    {
        for (int k = 0; k <= i; k++)
            p1[i] += p[k];
    }

    // histogram transformation  
    for (int x = 0; x<src1->width; x++)
    {
        for (int y = 0; y<src1->height; y++){
            uchar v = ((uchar*)(src1->imageData + src1->widthStep*y))[x];
               ((uchar*)(dst1->imageData + dst1->widthStep*y))[x] = p1[v] * 255 + 0.5;

		}
   }
    return 0;
}

void CRsImage::OnImageWallis()
{
	// TODO:  在此添加命令处理程序代码
	CString imagename = m_RsLayer->GetFileNameStr();
	CvxGdalWrap gdal1;

	gdal1.openRead(imagename);
	CvSize size1 = gdal1.size();

	Mat Image = Mat(size1.height, size1.width, CV_MAKETYPE(CV_8U, gdal1.channels()));
	gdal1.readImg(cvRect(0, 0, size1.width, size1.height), &Image);//转换为opencv格式

	Mat WallisFilterImage = wallis(Image);

	IplImage ipl_img(WallisFilterImage);
	cvSaveImage("wallis.tif", &ipl_img);
	MessageBox("wallis 滤波已完成", "卫星影像几何处理与颤振探测");

	m_RsLayer->ReadImageInfo("wallis.tif",
		customDef_imageWidth, customDef_imageHeight);//根据处理的影像生成数据集

	//图像数据的实时显示
	//创建窗口与绘图
	bool changed = false;
	bool showbandcountchange = false;

	if (m_rsFileMean == IMAGEFILE)
	{
		m_RsLayer->SetBlockBuffer(m_RsLayer,
			m_RsLayer->GetVWidth(),
			m_RsLayer->GetVHeight(),
			showbandcountchange || changed);

		m_RsLayer->ReadImageByBufferBlock(m_RsLayer,
			m_Left,
			m_Top,
			m_RsLayer->GetVWidth(),
			m_RsLayer->GetVHeight(),
			showbandcountchange || changed);
	}
	if (m_RsLayer->GetFileNameStr() != "" &&
		(m_RsLayer->GetImageHeight() > local_Height
		|| m_RsLayer->GetImageWidth() > local_Width))
	{
		if (m_RsScroll == NULL)
		{
			m_RsScroll = new CRsScroll();
			m_RsScroll->m_rsFileMean = m_rsFileMean;
			m_RsScroll->m_RsLayer = new CRsLayer(m_RsLayer->GetImageWidth(),
				m_RsLayer->GetImageHeight(),
				customDef_previewWidth,
				customDef_previewHeight,
				global_pDlgOperating->m_hWnd);
			m_RsScroll->m_RsLayer->RegisterSRD();
			m_RsScroll->m_ImageIndex = m_ImageIndex;

			m_RsScroll->m_ImageFileString = m_ImageFileString;
			m_RsScroll->Create(IDD_DIALOG_RSSCROLL, this);
			b_ShowRsScroll = true;
		}
		else
		{
			if (m_rsFileMean == IMAGEFILE)
			{
				m_RsScroll->FleshRsScroll();
				m_RsScroll->ShowWindow(SW_SHOW);
				b_ShowRsScroll = true;
			}
		}
	}
	if (m_RsLayer->GetFileNameStr() != "")
	{
		if (m_RsZoom == NULL)
		{
			m_RsZoom = new CRsZoom();
			m_RsZoom->m_rsFileMean = m_rsFileMean;
			m_RsZoom->m_RsLayer = new CRsLayer(m_RsLayer->GetImageWidth(),
				m_RsLayer->GetImageHeight(),
				customDef_zoomWidth,
				customDef_zoomHeight,
				global_pDlgOperating->m_hWnd);
			m_RsZoom->m_ImageIndex = m_ImageIndex;

			m_RsZoom->m_ImageFileString = m_ImageFileString;
			m_RsZoom->Create(IDD_DIALOG_RSZOOM, this);
			b_ShowRsZoom = true;
		}
		else
		{
			m_RsZoom->FleshRsZoom(m_Left + m_RectLeft,
				m_Top + m_RectTop,
				m_RectLeft,
				m_RectTop,
				m_RectWidthLen,
				m_RectHeightLen);
			m_RsZoom->ShowWindow(SW_SHOW);
			b_ShowRsZoom = true;
		}
	}
	if (iftie)
	{
		m_RsScroll->iftie = true;
		m_RsScroll->ReFleshWindow();
	}
	OnPaint();
}

