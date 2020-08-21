#include "../../3part/include.hpp"
#include <dirent.h>

int getdir(string dir, vector<string>& files) {
    DIR* dp;                //創立資料夾指標
    struct dirent* dirp;
    if ((dp = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)        //如果dirent指標非空
        files.push_back(dir + "/" + string(dirp->d_name));  //將資料夾和檔案名放入vector
    
    closedir(dp);                               //關閉資料夾指標
    return 0;
}

enum {
	RAW_8,
	RAW_10
};

enum {
    RGGB,
    BGGR,
    GRBG,
    GBRG
};

bool BayerConvert(cv::Mat& out_Img, BYTE* ori_Img, int iWidth, int iHeight, BYTE byFormat, BYTE bayerFormat)
{
	if (ori_Img == NULL)
		return false;

	BYTE* pRGB = NULL;
	int iImgWidth = iWidth;
	int iImgHeight = iHeight;
	int iSize = iWidth * iHeight;

	if (byFormat == RAW_8) {
		try {
			out_Img.create(iHeight, iWidth, CV_8UC1);
			out_Img.data = ori_Img;
			if (bayerFormat == RGGB)
				cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerRG2BGR);
			else if (bayerFormat == BGGR)
				cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerBG2BGR);
			else if (bayerFormat == GRBG)
				cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGR2BGR);
			else if (bayerFormat == GBRG)
				cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGB2BGR);
		}
		catch (const char* msg) {			
			cerr << msg << endl;
			return false;
		}
	}
    else if (byFormat == RAW_10) {
        try {
            out_Img.create(iHeight, iWidth, CV_8UC1);
            
			cv::Vec3b p;
			BYTE* pLineA, * pLineB;

			int i, j, real_j;
			int iLineSize = iWidth * 5 / 4; //因為10bits
			if (bayerFormat == RGGB)
			{
				for (i = 0; i < iHeight; i += 2)
				{
					pLineA = &ori_Img[i * iLineSize];
					pLineB = &ori_Img[(i + 1) * iLineSize];
					for (j = 0; j < iLineSize; j += 5)
					{
						try
						{
							p[0] = pLineB[j + 1];	// B
							p[1] = (pLineB[j] + pLineA[j + 1]) / 2;	// G
							p[2] = pLineA[j];		// R

							real_j = j * 4 / 5;

							out_Img.at<cv::Vec3b>(i, real_j) = out_Img.at<cv::Vec3b>(i + 1, real_j) =
								out_Img.at<cv::Vec3b>(i, real_j + 1) = out_Img.at<cv::Vec3b>(i + 1, real_j + 1) = p;

							p[0] = pLineB[j + 3];	// B
							p[1] = (pLineB[j + 2] + pLineA[j + 3]) / 2;	// G
							p[2] = pLineA[j + 2];		// R

							out_Img.at<cv::Vec3b>(i, real_j + 2) = out_Img.at<cv::Vec3b>(i + 1, real_j + 2) =
								out_Img.at<cv::Vec3b>(i, real_j + 3) = out_Img.at<cv::Vec3b>(i + 1, real_j + 3) = p;
						}
						catch (...)
						{
							string err = "BayerConvert Error at i :" + std::to_string(i) + " ,j :" + std::to_string(real_j);
							cout << err << endl;
						}
					}
				}
			}
			else if (bayerFormat == GRBG)
			{
				for (i = 0; i < iHeight; i += 2)
				{
					pLineA = &ori_Img[i * iLineSize];
					pLineB = &ori_Img[(i + 1) * iLineSize];
					for (j = 0; j < iLineSize; j += 5)
					{
						try
						{
							p[0] = pLineB[j];		// B
							p[1] = (pLineA[j] + pLineB[j + 1]) / 2;		// G
							p[2] = pLineA[j + 1];	// R

							real_j = j * 4 / 5;

							out_Img.at<cv::Vec3b>(i, real_j) = out_Img.at<cv::Vec3b>(i + 1, real_j) =
								out_Img.at<cv::Vec3b>(i, real_j + 1) = out_Img.at<cv::Vec3b>(i + 1, real_j + 1) = p;

							p[0] = pLineB[j + 2];		// B
							p[1] = (pLineA[j + 2] + pLineB[j + 3]) / 2;		// G
							p[2] = pLineA[j + 3];	// R

							out_Img.at<cv::Vec3b>(i, real_j + 2) = out_Img.at<cv::Vec3b>(i + 1, real_j + 2) =
								out_Img.at<cv::Vec3b>(i, real_j + 3) = out_Img.at<cv::Vec3b>(i + 1, real_j + 3) = p;
						}
						catch (...)
						{
							string err = "BayerConvert Error at i :" + std::to_string(i) + " ,j :" + std::to_string(real_j);
							cout << err << endl;
						}
					}
				}
			}
			else if (bayerFormat == BGGR)
			{
				for (i = 0; i < iHeight; i += 2)
				{
					pLineA = &ori_Img[i * iLineSize];
					pLineB = &ori_Img[(i + 1) * iLineSize];
					for (j = 0; j < iLineSize; j += 5)
					{
						try
						{
							//BGGR
							p[0] = pLineA[j];	// B
							p[1] = (pLineB[j] + pLineA[j + 1]) / 2;	// G
							p[2] = pLineB[j + 1];		// R

							real_j = j * 4 / 5;

							out_Img.at<cv::Vec3b>(i, real_j) = out_Img.at<cv::Vec3b>(i + 1, real_j) =
								out_Img.at<cv::Vec3b>(i, real_j + 1) = out_Img.at<cv::Vec3b>(i + 1, real_j + 1) = p;

							p[0] = pLineA[j + 2];	// B
							p[1] = (pLineB[j + 2] + pLineA[j + 3]) / 2;	// G
							p[2] = pLineB[j + 3];		// R

							out_Img.at<cv::Vec3b>(i, real_j + 2) = out_Img.at<cv::Vec3b>(i + 1, real_j + 2) =
								out_Img.at<cv::Vec3b>(i, real_j + 3) = out_Img.at<cv::Vec3b>(i + 1, real_j + 3) = p;
						}
						catch (...)
						{
							string err = "BayerConvert Error at i :" + std::to_string(i) + " ,j :" + std::to_string(real_j);
							cout << err << endl;
						}
					}
				}
			}
			else if (bayerFormat == GBRG)
			{
				for (i = 0; i < iHeight; i += 2)
				{
					pLineA = &ori_Img[i * iLineSize];
					pLineB = &ori_Img[(i + 1) * iLineSize];
					for (j = 0; j < iLineSize; j += 5)
					{
						try
						{
							p[0] = pLineA[j + 1]; 		// B
							p[1] = (pLineA[j] + pLineB[j + 1]) / 2;		// G
							p[2] = pLineB[j];// R

							real_j = j * 4 / 5;

							out_Img.at<cv::Vec3b>(i, real_j) = out_Img.at<cv::Vec3b>(i + 1, real_j) =
								out_Img.at<cv::Vec3b>(i, real_j + 1) = out_Img.at<cv::Vec3b>(i + 1, real_j + 1) = p;

							p[0] = pLineA[j + 3];		// B
							p[1] = (pLineA[j + 2] + pLineB[j + 3]) / 2;		// G
							p[2] = pLineB[j + 2];	// R

							out_Img.at<cv::Vec3b>(i, real_j + 2) = out_Img.at<cv::Vec3b>(i + 1, real_j + 2) =
								out_Img.at<cv::Vec3b>(i, real_j + 3) = out_Img.at<cv::Vec3b>(i + 1, real_j + 3) = p;
						}
						catch (...)
						{
							string err = "BayerConvert Error at i :" + std::to_string(i) + " ,j :" + std::to_string(real_j);
							cout << err << endl;
						}
					}
				}
			}
        }
		catch (const char* msg) {
			cerr << msg << endl;
			return false;
		}
    }
    else {
        cout << "byFormat doesn't support";
        return false;
    }
	
}

int main() {
    std::string dir = "../img";
    vector<string> files = vector<string>();
    getdir(dir, files);

	//2560 * 1920
	int img_width = 2560, imh_height = 1920;
	cout << "img_width : " << img_width << ", imh_height : " << imh_height << endl;

	//
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");

	for (int i = 0; i < files.size(); i++) {
		//輸出資料夾和檔案名稱於螢幕
		//cout << files[i] << endl;

		if (files[i].find("raw") != files[i].npos) {
			FILE* f_ptr;
			errno_t err;
			err = fopen_s(&f_ptr, files[i].c_str(), "rb");
			if (err != 0) {
				cout << "Open Failed :" << files[i] << endl;
			}
			else {
				int read_data_size = img_width * imh_height;	//RAW8, if RAW10 need * 2
				uchar* read_data = new uchar[read_data_size];
				if (read_data != NULL)
					size_t return_count = fread_s(read_data, read_data_size, read_data_size, 1, f_ptr);

				cv::Mat out_Img;
				if (BayerConvert(out_Img, read_data, img_width, imh_height, RAW_8, RGGB)) {
					files[i] = files[i].replace(files[i].find("raw"), 3, "jpg");
					cv::imwrite(files[i], out_Img);
				}
			}			
		}
	}
        



    std::system("pause");
    return 0;
}