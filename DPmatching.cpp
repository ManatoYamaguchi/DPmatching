#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

#define FILE_MAX 1
#define DIMENTION 15

class Onseifile
{
	public:
		string number;
		string name;
		string onso;
		int frame;
		double mcepdata[300][DIMENTION];
		int file_open(string city_num, string file_count);
};

//ファイルの読み込み、変数の読み込み
int Onseifile::file_open(string city_num, string file_count)
{
	number = file_count;

	ifstream fin("./city_mcepdata/city" + city_num 
			+ "/city" + city_num + "_" + number + ".txt");

	if(fin.fail())
	{
		cerr << "ファイルの読み込みに失敗\n";
		return -1;
	}

	fin >> name;
	fin >> onso;
	fin >> frame;

	for(int i = 0; i < frame; i++)
	{
		for(int j = 0; j < DIMENTION; j++)
			fin >> mcepdata[i][j];
	}

}

string s_file_count;

//テキストファイルの番号の型変換（int型→string型）
void type_conv(int file_count)
{
	if(file_count >= 0 and file_count < 9)
		s_file_count = "00" + to_string(file_count +1);

	else if(file_count >= 9 and file_count < 99)
		s_file_count = "0" + to_string(file_count +1);

	else if(file_count == 99)
		s_file_count = to_string(file_count +1);
}

double d[300][300];
double g[300][300];

//フレームごとの局所距離dを計算
void calc_d(int temp_frame, int michi_frame, 
		double temp_mcepdata[300][DIMENTION], double michi_mcepdata[300][DIMENTION])
{
	for(int i = 0; i < temp_frame; i++)
	{
		for(int j = 0; j < michi_frame; j++)
		{
			d[i][j] = 0;

			for(int k = 0; k < DIMENTION; k++)
			{
				d[i][j] += (temp_mcepdata[i][k] - michi_mcepdata[j][k]) 
						* (temp_mcepdata[i][k] - michi_mcepdata[j][k]);
				d[i][j] = sqrt(d[i][j]);
			}

			//cout << "d["<< i << "][" << j << "] : " << d[i][j] << '\n';	//debug
		}
	}
}

void calc_g(int temp_frame, int michi_frame)
{
	//初期条件g(0, 0)
	g[0][0] = d[0][0];
	
	//境界条件g(i, 0)とg(0, j)
	for(int i = 0; i < temp_frame; i++)
		g[i][0] = g[i -1][0] + d[i][0];

	for(int j = 0; j < michi_frame; j++)
		g[0][j] = g[0][j -1] + d[0][j];

	//累積距離gを計算
	for(int i = 0; i < temp_frame; i++)
	{
		for(int j = 0; j < michi_frame; j++)
		{
			double cand1 = g[i][j -1] + d[i][j];
			double cand2 = g[i -1][j -1] + 2 * d[i][j];
			double cand3 = g[i -1][j] + d[i][j];

			g[i][j] = cand1;
			if(cand2 < g[i][j])
				g[i][j] = cand2;
			if(cand3 < g[i][j])
				g[i][j] = cand3;

			//cout << "g[" << i << "][" << j << "] : " << g[i][j] << '\n';	//debug
		}
	}	
}

int main()
{
	string city_num1, city_num2;

	cout << "二つのファイルの認識精度を検証します。\n";

	cout << "一つ目のファイル：city";
	cin >> city_num1;

	cout << "二つ目のファイル：city";
	cin >> city_num2;

	cout << "city_num1 : " << city_num1 << '\n';	//debug
	cout << "city_num2 : " << city_num2 << '\n';	//debug

	for(int file_count = 0; file_count < FILE_MAX; file_count++)
	{
		type_conv(file_count);

		Onseifile temp_data;
		temp_data.file_open(city_num1, s_file_count);

		Onseifile michi_data;
		michi_data.file_open(city_num2, s_file_count);

		calc_d(temp_data.frame, michi_data.frame, temp_data.mcepdata, michi_data.mcepdata);

		calc_g(temp_data.frame, michi_data.frame);

		//単語間距離tの計算
		double t[file_count] = g[temp_data.frame -1][michi_data.frame -1] 
								/ (temp_data.frame + michi_data.frame);
		cout << "t[" << file_count << "] : " << t[file_count] << '\n';	//debug

		cout << "temp_name : " << temp_data.name << '\n';	//debug
		cout << "michi_name : " << michi_data.name << '\n';	//debug

		cout << "temp_1 : " << temp_data.mcepdata[0][0] << '\n';	//debug
		cout << "michi_1 : " << michi_data.mcepdata[0][0] << '\n';	//debug
	}
}
