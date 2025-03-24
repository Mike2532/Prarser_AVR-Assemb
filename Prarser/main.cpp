#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<queue>
using namespace std;

void string_letter_check(string str, uint16_t command, string name);
int letter_number(string code, char letter, uint16_t command, bool plus16, bool plus1);
void name_change(string& name, char letter, int number);
int letter_find(string name, char letter);
bool isR16_R31(string name);
bool isplus1(string comand);
void sign_check(int16_t& number, string code);
string get_queue_element(void);
uint16_t get_comand(void);
void comands_double_byte(uint16_t comand);

struct comand {
	string name;
	string code;
	int mask;
	int clear_mask;
};

vector<comand> get_comands_vector(string database_name);
queue<string> hexcomand;

int main(void)
{
	hexcomand.push("3D9A");
	hexcomand.push("0C94");
	hexcomand.push("3400");

	vector<comand> comands = get_comands_vector("database_one_byte.txt");

	uint16_t comand = get_comand();

	for (int i = 0; i < 120; i++)
	{
		if (comands[i].mask == (comand & comands[i].clear_mask))
		{
			string_letter_check(comands[i].code, comand, comands[i].name);
			break;
		}
		if (i == 119)
			comands_double_byte(comand);
	}


	return 0;
}

void string_letter_check(string code, uint16_t command, string name)
{
	bool plus16 = isR16_R31(name);
	bool plus1 = isplus1(name);
	vector<char> letters;
	for (int j = 15; j >= 0; j--)
	{
		if (code[j] != '0' && code[j] != '1' && (find(letters.begin(), letters.end(), code[j]) == end(letters)))
		{
			letters.push_back(code[j]);
			name_change(name, code[j], letter_number(code, code[j], command, plus16, plus1));
		}
	}
	int name_space = name.find('_');
	if (name_space != string::npos)
	{
		name.erase(name_space, 1);
		name.insert(name_space, " ");
	}
	cout << name;
}

int letter_number(string code, char letter, uint16_t command, bool plus16, bool plus1)
{
	int16_t number = 0;
	uint16_t counter = 0;
	for (int i = 15; i > 0; i--)
	{
		if (code[i] == letter)
		{
			counter++;
			if (command & (1 << (15 - i)))
				number |= (1 << (counter - 1));
		}
	}
	if (plus16 && letter == 'd')
		return number + 16;
	if (plus1)
	{
		sign_check(number, code);
		number *= 2; 
	}
	return number;
}

void name_change(string& name, char letter, int number)
{
	int letter_index = letter_find(name, letter);
	name.erase(letter_index, 1);
	name.insert(letter_index, to_string(number));
}

int letter_find(string name, char letter)
{
	for (int i = name.length() - 1; i > 0; i--)
	{
		if (name[i] == letter)
			return i;
	}
}

bool isR16_R31(string name)
{
	vector<string> changed = { "ldi_Rd,K", "andi_Rd,K", "ori_Rd,K", "subi_Rd,K", "sbci_Rd,K", "cbr_Rd,K", "ser_Rd", "!"};
	bool finded = false;
	if (find(changed.begin(), changed.end(), name) != changed.end())
		return true;
	return false;
}

bool isplus1(string comand)
{
	vector<string> pluses = { "brbc_s,k", "brbs_s,k", "brcc_k", "brcs_k", "brsh_k", "brlo_k", "brne_k",
								"breq_k", "brpl_k", "brmi_k", "brvc_k", "brvs_k", "brge_k", "brlt_k",
								"brhc_k", "brhs_k", "brtc_k", "brts_k", "brid_k", "brie_k",
								"rcall_k", "rjmp_k", "!" };
	bool finded = false;
	if (find(pluses.begin(), pluses.end(), comand) != pluses.end())
		return true;
	return false;
}

void sign_check(int16_t& number, string code)
{
	if (code[2] == '0')
	{//rjmp - 12bit data
		if (number & (1 << 11))
			number |= 0xF000;
	}
	else
	{//not rjmp - 7bit data
		if (number & (1 << 6))
			number |= 0xFF80;
	}
}

string get_queue_element(void)
{
	string ans = hexcomand.front(); 
	hexcomand.pop();
	return ans;
}

uint16_t get_comand(void)
{
	string str = get_queue_element();

	unsigned char b;

	for (int i = 0; i < 2; i++)
	{
		b = str[0];
		for (int j = 1; j < 4; j++)
			str[j - 1] = str[j];
		str[3] = b;
	}

	uint16_t comand = 0;

	for (int i = 0; i < 4; i++)
	{
		if (str[i] >= '0' && str[i] <= '9')
			b = str[i] - '0';
		else
			b = str[i] - ('A' - 0x0A);
		comand = (comand << 4) | b;
	}
	
	return comand;
}

vector<comand> get_comands_vector(string database_name)
{
	vector<comand> comands;
	string name; 
	string code; 
	uint16_t mask = 0; 
	uint16_t clear_mask = 0;
	
	ifstream file(database_name);
	
	while (file >> name >> code >> mask >> clear_mask)
		comands.push_back({ name, code, mask, clear_mask });
	file.close();

	return comands;
}

void comands_double_byte(uint16_t comand16)
{
	uint32_t comand32 = (comand16 << 16);
	comand16 = get_comand();
	comand32 |= comand16;

	vector<comand> comands;

}