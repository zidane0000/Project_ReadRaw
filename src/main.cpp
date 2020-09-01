//fix compile error 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
#pragma warning( disable : 4996 )
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
    return 1;
}

enum {
	RAW_8 = 0,
	RAW_10,		//     A,      B,      C,      D, Low
				//									|
				//bit0,1, bit2,3, bit4,5, bit6,7 <---
	RAW_16		// H,L,H,L or L,H,L,H
};

enum {
    RGGB,
    BGGR,
    GRBG,
    GBRG
};

enum {
	Gray = 0,
	RGB,
};

bool BayerConvert(cv::Mat& out_Img, BYTE* ori_Img, int iWidth, int iHeight, int byFormat, int bayerFormat, int targetformat)
{
	if (ori_Img == NULL)
		return false;

	BYTE* pRGB = NULL;
	int iImgWidth = iWidth;
	int iImgHeight = iHeight;
	int iSize = iWidth * iHeight;
	
	if (targetformat == Gray)
		out_Img.create(iHeight, iWidth, CV_8UC1);
	else if (targetformat == RGB)
		out_Img.create(iHeight, iWidth, CV_8UC3);
	else {
		cout << "targetformat is not yet supported" << endl;
		return false;
	}		
	
	if (byFormat == RAW_8) {
		try {						
			out_Img.data = ori_Img;
			if (targetformat == Gray) {
				if (bayerFormat == RGGB)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerRG2GRAY);
				else if (bayerFormat == BGGR)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerBG2GRAY);
				else if (bayerFormat == GRBG)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGR2GRAY);
				else if (bayerFormat == GBRG)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGB2GRAY);
			}
			else if (targetformat == RGB) {
				if (bayerFormat == RGGB)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerRG2BGR);
				else if (bayerFormat == BGGR)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerBG2BGR);
				else if (bayerFormat == GRBG)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGR2BGR);
				else if (bayerFormat == GBRG)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGB2BGR);
			}
		}
		catch (const char* msg) {			
			cerr << msg << endl;
			return false;
		}
	}
    else if (byFormat == RAW_10) {
		//     A,      B,      C,      D, Low
		//									|
		//bit0,1, bit2,3, bit4,5, bit6,7 <---
		//So if you don't want to show Low byte(because it's very small), you can reference the below code
		if (targetformat != RGB) {
			cout << "RAW_10 only support RGB" << endl;
			return false;
		}

		try {			
			cv::Vec3b p;
			BYTE* pLineA, * pLineB;
			int i, j, real_j;
			int iLineSize = iWidth * 5 / 4; //因為10bits
			if (bayerFormat == RGGB) {
				for (i = 0; i < iHeight; i += 2) {
					pLineA = &ori_Img[i * iLineSize];
					pLineB = &ori_Img[(i + 1) * iLineSize];
					for (j = 0; j < iLineSize; j += 5) {
						try	{
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
						catch (...)	{
							string err = "BayerConvert Error at i :" + std::to_string(i) + " ,j :" + std::to_string(real_j);
							cout << err << endl;
						}
					}
				}
			}
			else if (bayerFormat == GRBG) {
				for (i = 0; i < iHeight; i += 2) {
					pLineA = &ori_Img[i * iLineSize];
					pLineB = &ori_Img[(i + 1) * iLineSize];
					for (j = 0; j < iLineSize; j += 5) {
						try	{
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
						catch (...)	{
							string err = "BayerConvert Error at i :" + std::to_string(i) + " ,j :" + std::to_string(real_j);
							cout << err << endl;
						}
					}
				}
			}
			else if (bayerFormat == BGGR) {
				for (i = 0; i < iHeight; i += 2) {
					pLineA = &ori_Img[i * iLineSize];
					pLineB = &ori_Img[(i + 1) * iLineSize];
					for (j = 0; j < iLineSize; j += 5) {
						try {
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
						catch (...) {
							string err = "BayerConvert Error at i :" + std::to_string(i) + " ,j :" + std::to_string(real_j);
							cout << err << endl;
						}
					}
				}
			}
			else if (bayerFormat == GBRG) {
				for (i = 0; i < iHeight; i += 2) {
					pLineA = &ori_Img[i * iLineSize];
					pLineB = &ori_Img[(i + 1) * iLineSize];
					for (j = 0; j < iLineSize; j += 5) {
						try {
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
						catch (...) {
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
	else if (byFormat == RAW_16) {
		try {
			ushort tmp;
			int i, j;
			for (i = 0; i < iHeight; i++)
				for (j = 0; j < iWidth; j++) {
					tmp = ori_Img[iWidth * 2 * i + j * 2] >> 2;
					tmp += ori_Img[iWidth * 2* i + j * 2 + 1] << 6;
					out_Img.at<uchar>(i, j) = tmp;
				}

			if (targetformat == Gray) {
				if (bayerFormat == RGGB)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerRG2GRAY);
				else if (bayerFormat == BGGR)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerBG2GRAY);
				else if (bayerFormat == GRBG)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGR2GRAY);
				else if (bayerFormat == GBRG)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGB2GRAY);
			}
			else if (targetformat == RGB) {
				if (bayerFormat == RGGB)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerRG2BGR);
				else if (bayerFormat == BGGR)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerBG2BGR);
				else if (bayerFormat == GRBG)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGR2BGR);
				else if (bayerFormat == GBRG)
					cv::cvtColor(out_Img, out_Img, cv::COLOR_BayerGB2BGR);
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
	cout << "This Tool is convert RawImage(Please notice the Extension need to bo lowercase) to JPG or PNG" << endl;
	std::string dir = "../img";
	cout << "Target folder(current folder is '.') : ";
	cin >> dir;
    vector<string> files = vector<string>();
	if (getdir(dir, files) == errno) {
		std::system("pause");
		return 0;
	}

	fstream file;
	smatch m;
	regex reg_WxH("([0-9]*)[xX]([0-9]*)");
	int img_width = 2560, imh_height = 1920;
	if (regex_search(dir, m, reg_WxH)) {
		img_width = stoi(((ssub_match)m[1]).str());
		imh_height = stoi(((ssub_match)m[2]).str());
	}
	else {
		cout << "img_width : ";
		cin >> img_width;
		cout << "imh_height : ";
		cin >> imh_height;		
	}

	cout << "img_width : " << img_width << ", imh_height : " << imh_height << endl;
	int byformat = RAW_8;
	cout << "byformat(0:RAW_8,1:RAW_10,2:RAW_16) : ";
	cin >> byformat;

	int bayerformat = RGGB;
	cout << "bayerformat(0:RGGB,1:BGGR,2:GRBG,3:GBRG) : ";
	cin >> bayerformat;

	int targetformat = RGGB;
	cout << "targetformat(0:Gray,1:RGB) : ";
	cin >> targetformat;

	cout << "-------convert start-------" << endl;
	for (int i = 0; i < files.size(); i++) {
		//輸出資料夾和檔案名稱於螢幕
		if (files[i].find("raw") != files[i].npos) {
			cout << files[i] << endl;
			FILE* f_ptr;
			errno_t err;
			err = fopen_s(&f_ptr, files[i].c_str(), "rb");
			if (err != 0)
				cout << "Open Failed :" << files[i] << endl;
			else {
				int read_data_size = img_width * imh_height;
				if (byformat == RAW_10)
					read_data_size = read_data_size * 5 / 4;
				else if (byformat == RAW_16)
					read_data_size *= 2;

				uchar* read_data = new uchar[read_data_size];
				if (read_data != NULL) {
					size_t return_count = fread_s(read_data, read_data_size, sizeof(uchar), read_data_size / sizeof(uchar), f_ptr);
					if(return_count * sizeof(uchar) != read_data_size)
						cout << "Waring!!Read_data_size is different, please check raw image" << endl;
				}

				cv::Mat out_Img;
				if (BayerConvert(out_Img, read_data, img_width, imh_height, byformat, bayerformat, targetformat)) {
					files[i] = files[i].replace(files[i].find("raw"), 3, "jpg");
					cv::imwrite(files[i], out_Img);
				}
			}			
		}
	}
	cout << "--------convert end--------" << endl;
    std::system("pause");
    return 0;
}