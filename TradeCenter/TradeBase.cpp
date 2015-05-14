#include "TradeBase.h"

//Function to encrypt important information.
//Function type: int. Return the length of data to be encrypted, calculated by bytes.
//Parameters:
//pszEncrypted: the encrypted data string.
//pszOriginal: the original data string.
//pszPublicKey: Public key string.
//pszEncryptType: Encrypt type.
// ���ݼ���(���Ļ�������ԭ�ģ���Կ����������)
int Encrypt_org(char *pszEncrypted,  const char *pszOriginal, const char *pszPublicKey, const char *pszEncryptType)
{	
	int iType = atoi(pszEncryptType);
	int iRetSize = 0;

	switch (iType)
	{
	case 0:		//������
		{
			strcpy(pszEncrypted, pszOriginal);
			iRetSize = strlen(pszOriginal);
		}
		break;
	case 2:		//des����
		{
			//DES����ר�ñ�������ʼ����
			DES_cblock stKey = {0};
			DES_string_to_key(pszPublicKey, &stKey);
			DES_key_schedule stSchedule = {0};

			if(DES_set_key_checked(&stKey, &stSchedule) == 0)
			{
				// 1.���ܵ����ı�����8λ��
				// 2.ֻ�ܼ���8λ������ж��飬��Ҫ��������DES_ecb_encrypt
				// 3.�����λʱ���ʽΪ0x00����C������ַ�Ϊ0x00���˴�δ�ٽ��в������
				int iBlockCount = 0;
				int iDataLength = strlen(pszOriginal);

				if (iDataLength % 8 == 0)
				{
					iBlockCount = iDataLength / 8;
				}
				else
				{
					iBlockCount = (iDataLength / 8) + 1;
				}
				for (int iIndex = 0; iIndex < iBlockCount; ++iIndex)
				{
					DES_ecb_encrypt((const_DES_cblock *)(pszOriginal + iIndex*8), (DES_cblock *)(pszEncrypted + iIndex*8), &stSchedule, DES_ENCRYPT);
				}
				iRetSize = iBlockCount * 8;
			}
		}
		break;
	case 101:// blowflish����
		{
			BF_KEY stBF_Key = {0};     
			char ivec[32] = {0};
			BF_set_key(&stBF_Key, strlen(pszPublicKey), (const unsigned char *)pszPublicKey);
			int nNum = 0;
			int nOutBuffLen = strlen(pszOriginal);// 
			BF_cfb64_encrypt((const unsigned char *)pszOriginal, (unsigned char *)pszEncrypted, nOutBuffLen, &stBF_Key, (unsigned char *)&ivec, &nNum, BF_ENCRYPT);
			iRetSize = nOutBuffLen;
		}
		break;
	default:	
		std::cout << "Wrong Encrypt Type\n";
		exit(2);
	}
	return(iRetSize);
}

// ���л����ݣ�ԭ�ģ�ԭ�ĳ��ȣ������������������������ȣ�
char *SerializeData(const char *pszDataSrc, int iSizeSrc, char *pszDataDest, int iSizeDest)
{//���л�����
	memset(pszDataDest, 0, iSizeDest);
	char szValue[64] = {0};
	for (int iIndex = 0; iIndex < iSizeSrc; ++iIndex)
	{
		_snprintf(szValue, sizeof(szValue) - 1, "%02x", (unsigned char)pszDataSrc[iIndex]);
		if (iIndex*2 >= iSizeDest) 
			break;
		strcat(pszDataDest, szValue);
	}
	return pszDataDest;
}

// ����Ψһ��
std::string GenerateUniqueID()
{
	TradeTime t = time(0); 
	static long nRound = rand()*10;

	std::stringstream ss;
	ss << &t << t << ++nRound;
	return ss.str();
}


NQ::EnumParser<NQ::Currency>::EnumParser()
{
	enumMap["CNY"] = Currency::CNY;
	enumMap["USD"] = Currency::USD;
	enumMap["HKD"] = Currency::HKD;
}
NQ::EnumParser<NQ::MarketCode>::EnumParser()
{
	enumMap["CNY"] = MarketCode::XSHG;
	enumMap["USD"] = MarketCode::XSHE;
}
NQ::EnumParser<NQ::PositionType>::EnumParser()
{
	enumMap["SB"] = PositionType::ShareBalance;
	enumMap["SAV"] = PositionType::ShareAvailable;
	enumMap["SQ"] = PositionType::ShareQuantity;
	enumMap["LB"] = PositionType::LastBalance;
	enumMap["SS"] = PositionType::SoldFrozon;
	enumMap["SF"] = PositionType::ManualFrozon;
	enumMap["SBQ"] = PositionType::BuyQuantity;
}
NQ::EnumParser<NQ::PosAmtType>::EnumParser()
{
	enumMap["FB"] = PosAmtType::FundBalance;
	enumMap["FAV"] = PosAmtType::FundAvailable;
	enumMap["MV"] = PosAmtType::MainValue;
	enumMap["F"] = PosAmtType::Fund;
	enumMap["SV"] = PosAmtType::CurentValue;
	enumMap["FBF"] = PosAmtType::FundBoughtFrozon;
	enumMap["BC"] = PosAmtType::CurrentCost;
	enumMap["SMV"] = PosAmtType::SingleMarketValue;
	enumMap["IC"] = PosAmtType::ProfitLost;
	enumMap["PC"] = PosAmtType::PositionCost;
	enumMap["BPL"] = PosAmtType::BoughtProfitLost;
}