#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> 

using namespace std;

#define FILE_MAX 2
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

int Onseifile::file_open(string city_num, string file_count)
{
	number = file_count;

	ifstream ifs("./city_mcepdata/city" + city_num 
			+ "/city" + city_num + "_" + number + ".txt");

	if(ifs.fail())
	{
		cout << "ファイルの読み込みに失敗\n";
		return -1;
	}

	ifs >> name;
	ifs >> onso;
	ifs >> frame;

	for(int i = 0; i < frame; i++)
	{
		for(int j = 0; j < DIMENTION; j++)
			ifs >> mcepdata[i][j];
	}

}

void d_debug(string name, double val)
{
	cout << name << " : " << val << '\n';
}

void s_debug(string name, string val)
{
	cout << name << " : " << val << '\n';
}

string s_file_count;

void type_conv(int file_count)
{
	if(file_count >= 0 and file_count < 9)
		s_file_count = "00" + to_string(file_count +1);

	else if(file_count >= 9 and file_count < 99)
		s_file_count = "0" + to_string(file_count +1);

	else if(file_count == 99)
		s_file_count = to_string(file_count +1);
}

int main()
{
	string city_num1, city_num2;

	cout << "二つのファイルの認識精度を検証します。\n";

	cout << "一つ目のファイル：city";
	cin >> city_num1;

	cout << "二つ目のファイル：city";
	cin >> city_num2;

	s_debug("city_num1", city_num1);
	s_debug("city_num2", city_num2);

	for(int file_count = 0; file_count < FILE_MAX; file_count++)
	{
		type_conv(file_count);

		Onseifile temp_data;
		temp_data.file_open(city_num1, s_file_count);

		Onseifile michi_data;
		michi_data.file_open(city_num2, s_file_count);

		s_debug("temp_name", temp_data.name);
		s_debug("michi_name", michi_data.name);

		d_debug("temp_1", temp_data.mcepdata[0][0]);
		d_debug("michi_1", michi_data.mcepdata[0][0]);
	}
}
