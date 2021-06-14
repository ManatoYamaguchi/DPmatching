#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

#define FILE_MAX 100
#define DIMENTION 15
#define MAXSIZE 256

//各パラメータの定義
double d[MAXSIZE][MAXSIZE];
double g[MAXSIZE][MAXSIZE];
double t[FILE_MAX];

//ファイルのクラスの定義
class Onseifile
{
	public:
		string name;
		string onso;
		int frame;
		double mcepdata[MAXSIZE][DIMENTION];
		int file_open(string city_num, int file_count);

};

//ファイルの読み込み、変数の読み込み
int Onseifile::file_open(string city_num, int file_count)
{
	string file_num;

	if(file_count >= 0 and file_count < 9)
		file_num = "00" + to_string(file_count +1);

	else if(file_count >= 9 and file_count < 99)
		file_num = "0" + to_string(file_count +1);

	else if(file_count == 99)
		file_num = to_string(file_count +1);


	ifstream fin("./city_mcepdata/city" + city_num 
			+ "/city" + city_num + "_" + file_num + ".txt");

	if(fin.fail())
	{
		cout << "./city_mcepdata/city" + city_num + "/city"
			+ city_num + "_" + file_num + ".txtの読み込みに失敗\n\n";
		return 1;
	}

	fin >> name;
	fin >> onso;
	fin >> frame;

	for(int i = 0; i < frame; i++)
	{
		for(int j = 0; j < DIMENTION; j++)
			fin >> mcepdata[i][j];
	}

	return 0;
}

//フレームごとの局所距離 d を計算
void calc_d(int temp_frame, int michi_frame, 
		double temp_mcepdata[MAXSIZE][DIMENTION], double michi_mcepdata[MAXSIZE][DIMENTION])
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
			}

			d[i][j] = sqrt(d[i][j]);

			//cout << "d["<< i << "][" << j << "] : " << d[i][j] << '\n';	//debug
		}
	}
}

//累積距離 g の導出
void calc_g(int temp_frame, int michi_frame)
{
	//初期条件 g(0, 0)
	g[0][0] = d[0][0];
	
	//境界条件 g(i, 0) と g(0, j) の決定
	for(int i = 1; i < temp_frame; i++)
		g[i][0] = g[i -1][0] + d[i][0];

	for(int j = 1; j < michi_frame; j++)
		g[0][j] = g[0][j -1] + d[0][j];

	//累積距離 g を導出
	for(int i = 1; i < temp_frame; i++)
	{
		for(int j = 1; j < michi_frame; j++)
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

//単語間距離 t の導出
void calc_t(int file_count, int temp_frame, int michi_frame)
{
		t[file_count] = g[temp_frame -1][michi_frame -1] / (temp_frame + michi_frame);

		//cout << "t[" << file_count << "] : " << t[file_count] << '\n';	//debug
}

//一致回数のカウント
int matching(int file_count)
{
	double t_min = t[0];
	int t_min_num = 0;

	//最小の単語間距離 t_min の導出
	for(int num_count = 0; num_count < FILE_MAX; num_count++)
	{
		if(t_min > t[num_count])
		{
			t_min = t[num_count];
			t_min_num = num_count;
		}
	}

	//cout << "t_min : " << t_min << '\n';	//debug
	
	if(t_min_num == file_count)
	{
		//cout << "ari "<< flush;
		return 1;
	}
	else
	{
		//cout << "muda "<< flush;
		return 0;
	}
	
}

//プログラムの続行/終了の判定
int continue_judg(char judg_moji)
{
	while(1)
	{
		if(judg_moji == 'Y' or judg_moji == 'y')
		{
			cout << "\n再度";
			return 1;
		}
		else if(judg_moji == 'N' or judg_moji == 'n')
		{
			cout << "プログラムを終了します。\n";
			return 0;
		}
		else
		{
			cout << "Y : yes / N : no   :";
			cin >> judg_moji;
		}
	}
}

int main()
{
	char judg_moji;

	do
	{
		int match_count = 0;
		
		string city_num1, city_num2;

		cout << "二つのファイルの認識精度を検証します。\n";

		cout << "テンプレートファイル：city";
		cin >> city_num1;

		cout << "　　　　未知ファイル：city";
		cin >> city_num2;

		//cout << "city_num1 : " << city_num1 << '\n';	//debug
		//cout << "city_num2 : " << city_num2 << '\n';	//debug

		for(int temp_file_count = 0; temp_file_count < FILE_MAX; temp_file_count++)
		{
			Onseifile temp_data;
			switch(temp_data.file_open(city_num1, temp_file_count))
			{
				case 0: break;
				case 1: goto Exit; break;
			}

			for(int michi_file_count = 0; michi_file_count < FILE_MAX; michi_file_count++)
			{
				Onseifile michi_data;
				switch(michi_data.file_open(city_num2, michi_file_count))
				{
					case 0: break;
					case 1: goto Exit; break;
				}
				

				calc_d(temp_data.frame, michi_data.frame, temp_data.mcepdata, michi_data.mcepdata);

				calc_g(temp_data.frame, michi_data.frame);

				calc_t(michi_file_count, temp_data.frame, michi_data.frame);
			}

			//一致回数の導出
			match_count += matching(temp_file_count);

			//cout << "temp_name : " << temp_data.name << '\n';	//debug
			//cout << "michi_name : " << michi_data.name << '\n';	//debug

			//cout << "temp_1 : " << temp_data.mcepdata[0][0] << '\n';	//debug
			//cout << "michi_1 : " << michi_data.mcepdata[0][0] << '\n';	//debug
		}

		//cout << "match_count : " << match_count << '\n';	//debug

		cout << "\n-----result-----\n";

		//一致率の導出
		double match_par;

		match_par = static_cast<double>(match_count) / FILE_MAX * 100;
		cout << "一致率：" << match_par << " %\n\n";

		Exit:

		//次の試行の有無
		cout << "再度検証を行いますか？　　Y : yes / N : no   :";
		cin >> judg_moji;

	} while(continue_judg(judg_moji));
}
