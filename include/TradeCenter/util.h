#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <algorithm>
#include <functional>

namespace NQ{

	/**********************************************************
	*
	*���ܣ�ȥ��ո�
	*
	*str��Դ�ַ���
	*
	*����ֵ��ȥ����ո����ַ���
	*
	***********************************************************/
	std::string &rtrim(std::string &str){
		str.erase(std::find_if(str.rbegin(), str.rend(),
			std::not1(std::ptr_fun(::isspace))).base(),
			str.end());
		return str;

	}
	/**********************************************************
	*
	*���ܣ�ȥǰ�ո�
	*
	*str��Դ�ַ���
	*
	*����ֵ��ȥ��ǰ�ո����ַ���
	*
	***********************************************************/
	std::string &ltrim(std::string &str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(),
			std::not1(std::ptr_fun(::isspace))));
		return str;

	}
	/**********************************************************
	*
	*���ܣ�ȥǰ��ո�
	*
	*str��Դ�ַ���
	*
	*����ֵ��ȥ��ǰ��ո����ַ���
	*
	***********************************************************/
	std::string &trim(std::string &str)
	{
		return rtrim(ltrim(str));
	}


	/**********************************************************
	*
	*���ܣ��Ƚ����ݣ������һ���򸲸�
	*
	*from: Դ����
	*to: ��������
	*flag: �Ƿ�һ��
	*count: ����ĳ���
	*
	***********************************************************/
	void ifNotCmpThenCpy(unsigned int& from, unsigned int& to, bool& flag)
	{
		if(from != to){
			flag = true;
			from = to;
		}
	}
	void ifNotCmpThenCpy(int& from, int& to, bool& flag)
	{
		if(from != to){
			flag = true;
			from = to;
		}
	}
	void ifNotCmpThenCpy(__int64& from, __int64& to, bool& flag)
	{
		if(from != to){
			flag = true;
			from = to;
		}
	}
	void ifNotCmpThenCpy(char* from, char* to, bool& flag, int size)
	{
		if(strcmp(from,to)!=0){
			flag = true;
			strcpy_s(to, size, from);
		}
	}
	void ifNotCmpThenCpy(unsigned int* from, unsigned int* to, bool& flag, int count)
	{
		for (int i = 0; i < count; i++){
			if (*(from+sizeof(unsigned int)*i) != *(to+sizeof(unsigned int)*i))
			{
				flag = true;
				*(to+sizeof(unsigned int)*i) = *(from+sizeof(unsigned int)*i);
			}
		}
	}

	void intArrayToDouble(unsigned int* from, double* to, int count, int divide){
		for (int i = 0; i < count; i++)
		{
			if (!from[i]) continue;
			to[i] = (from[i]*1.0)/divide;
		}
	}
}

#endif // !UTIL_H
