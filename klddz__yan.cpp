#include <Windows.h>
#include <iostream>
#include <assert.h>
using namespace std;

#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序
#define ST_CUSTOM					2									//自定排序
#define MAX_COUNT					20									//最大数目

#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_TAKE_ONE			7									//三带一单
#define CT_THREE_TAKE_TWO			8									//三带一对
#define CT_FOUR_TAKE_ONE			9									//四带两单
#define CT_FOUR_TAKE_TWO			10									//四带两对
#define CT_BOMB_CARD				11									//炸弹类型
#define CT_MISSILE_CARD				12									//火箭类型
#define MAX_TYPE_COUNT 254



struct tagOutCardTypeResult 
{
	BYTE							cbCardType;							//扑克类型
	BYTE							cbCardTypeCount;					//牌型数目
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];//每手个数
	BYTE							cbCardData[MAX_TYPE_COUNT][MAX_COUNT];//扑克数据
};

struct tagAnalyseResult
{
    BYTE    cbBlockCount[4];
    BYTE    cbCardData[4][20];
};

void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult& AnalyseResult);

BYTE GetCardValue(BYTE cbCardValue)
{
    return cbCardValue & 0x0F;
}

int CountArray(BYTE cbCardData[])
{
    int Count = 0;
    if(cbCardData == NULL)
    {
        return Count;
    }
    int index = 0;
    while()
}

bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	assert(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount = 0, cbTempCardData[MAX_COUNT];
	if (cbCardCount > CountArray(cbTempCardData)) return false;

	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

BYTE GetCardLogicValue(BYTE cbCardData)
{
    BYTE cbCardColor = cbCardData & 0xF0;
    BYTE cbCardValue = cbCardData& 0x0F;

    if (cbCardColor == 0x40) return cbCardValue + 2;
    return (cbCardValue <= 2) ? (cbCardValue + 13) : cbCardValue;
}

void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
    if(cbCardCount == 0) return;
    if(cbSortType == ST_CUSTOM) return;

    BYTE cbSortValue[MAX_COUNT];
    for(BYTE i = 0; i < cbCardCount; i++) cbSortValue[i] = GetCardLogicValue(cbCardData[i]);

    bool bSorted = true;
    BYTE cbSwitchData = 0, cbLast = cbCardCount - 1;
    do
    {
        bSorted = true;
        for(BYTE i = 0; i < cbLast; i++)
        {
            if((cbSortValue[i] < cbSortValue[i+1]) ||
            ((cbSortValue[i] == cbSortValue[i+1]) && (cbCardData[i] < cbCardData[i+1])))
            {
                bSorted = false;
                cbSwitchData = cbCardData[i];
                cbCardData[i] = cbCardData[i+1];
                cbCardData[i+1] = cbSwitchData;

                cbSwitchData = cbSortValue[i];
                cbSortValue[i] = cbSortValue[i+1];
                cbSortValue[i+1] = cbSwitchData;
            }
        }
        cbLast--;
    }while(bSorted == false);

    return;
}

void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult& AnalyseResult)
{
    memset(&AnalyseResult, 0, sizeof(AnalyseResult));

    for(BYTE i = 0; i < cbCardCount; i++)
    {
        BYTE cbSameCount = 1, cbCardValueTemp = 0;
        BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);

        for(BYTE j = i + 1; j < cbCardCount; j++)
        {
            if(GetCardLogicValue(cbCardData[j]) != cbLogicValue) break;

            cbSameCount++;
        }

        BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount - 1]++;
        for(BYTE j = 0; j < cbSameCount; j++) AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount + j] = cbCardData[i+j];

        i += cbSameCount - 1;
    }
    return;
}

/*void GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount)
{
	BYTE cbTmpCard[MAX_COUNT] ;
	memcpy(cbTmpCard, cbHandCardData, cbHandCardCount) ;
	//大小排序
	SortCardList(cbTmpCard, cbHandCardCount, ST_ORDER) ;

	cbLineCardCount = 0 ;

	//数据校验
	if(cbHandCardCount<5) return ;

	BYTE cbFirstCard = 0 ;
	//去除2和王
	for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCard[i])<15)	{cbFirstCard = i ; break ;}

	BYTE cbSingleLineCard[12] ;
	BYTE cbSingleLineCount=0 ;
	BYTE cbLeftCardCount = cbHandCardCount ;
	bool bFindSingleLine = true ;

	//连牌判断
	while (cbLeftCardCount>=5 && bFindSingleLine)
	{
		cbSingleLineCount=1 ;
		bFindSingleLine = false ;
		BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
		cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[cbFirstCard] ;
		for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
		{
			BYTE cbCardData=cbTmpCard[i];

			//连续判断
			if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && 
            GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
			{
				cbLastCard = cbTmpCard[i] ;
				if(cbSingleLineCount<5) 
				{
					cbSingleLineCount = 1 ;
					cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[i] ;
					continue ;
				}
				else break ;
			}
			//同牌判断
			else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
			{
				cbLastCard = cbCardData ;
				cbSingleLineCard[cbSingleLineCount] = cbCardData ;
				++cbSingleLineCount ;
			}					
		}

		//保存数据
		if(cbSingleLineCount>=5)
		{
			RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCard, cbLeftCardCount);
			memcpy(cbLineCardData+cbLineCardCount , cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount) ;
			cbLineCardCount += cbSingleLineCount ;
			cbLeftCardCount -= cbSingleLineCount ;
			bFindSingleLine = true ;
		}
	}
}*/

BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
    switch(cbCardCount)
    {
        case 0:
            return CT_ERROR;
        case 1:
            return CT_SINGLE;
        case 2:
            {
                if((cbCardData[0] == 0x4E) && (cbCardData[1] == 0x4F)) return CT_MISSILE_CARD;
                if(GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;

                return CT_ERROR;
            }
    }

    tagAnalyseResult AnalyseResult;
    AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

    if(AnalyseResult.cbBlockCount[3] > 0)
    {
        if((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 4)) return CT_BOMB_CARD;
        if((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 6)) return CT_FOUR_TAKE_ONE;
        if((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 8) && (AnalyseResult.cbBlockCount[1] == 2)) return CT_FOUR_TAKE_ONE;
    }

    if(AnalyseResult.cbBlockCount[2] > 0)
    {
        //有两组或以上的三牌
        if(AnalyseResult.cbBlockCount[2] > 1)
        {
            BYTE cbCardData = AnalyseResult.cbCardData[2][0];
            BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

            if(cbFirstLogicValue >= 15) return CT_ERROR;

            for(BYTE i = 1; i < AnalyseResult.cbBlockCount[2]; i++)
            {
                BYTE cbCardData = AnalyseResult.cbCardData[2][i*3];
                if(cbFirstLogicValue != GetCardLogicValue(cbCardData) + i)
                    return CT_ERROR;
            }
        }
        else if(cbCardCount == 3) return CT_THREE;

        if(AnalyseResult.cbBlockCount[2]*3 == cbCardCount) return CT_THREE_LINE;
        if(AnalyseResult.cbBlockCount[2]*4 == cbCardCount) return CT_THREE_TAKE_ONE;
        if((AnalyseResult.cbBlockCount[2]*5 == cbCardCount) && (AnalyseResult.cbBlockCount[1] == AnalyseResult.cbBlockCount[2])) return CT_THREE_TAKE_TWO;

        return CT_ERROR;
    }

    //双顺类型
    if(AnalyseResult.cbBlockCount[1] >= 3)
    {
        BYTE cbCardCData = AnalyseResult.cbCardData[1][0];
        BYTE cbFirstLogicValue = GetCardLogicValue(cbCardCData);

        if(cbFirstLogicValue >= 15) return CT_ERROR;

        for(BYTE i=1; i < AnalyseResult.cbBlockCount[i]; i++)
        {
            BYTE cbCardData = AnalyseResult.cbCardData[1][i*2];
            if(cbFirstLogicValue != (GetCardLogicValue(cbCardCData) + i)) return CT_ERROR;
        }

        if((AnalyseResult.cbBlockCount[1]*2) == cbCardCount) return CT_DOUBLE_LINE;

        return CT_ERROR;
    }

    if((AnalyseResult.cbBlockCount[0] > 5) && (AnalyseResult.cbBlockCount[0] == cbCardCount))
    {
        BYTE cbCardData = AnalyseResult.cbCardData[0][0];
        BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

        if(cbFirstLogicValue >= 15) return CT_ERROR;

        for(BYTE i = 1; i < AnalyseResult.cbBlockCount[0]; i++)
        {
            BYTE cbCardData = AnalyseResult.cbCardData[0][i];
            if(cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
        }

        return CT_SINGLE_LINE;
    }

    return CT_ERROR;
}

void GetAllBombCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBombCardData[], BYTE &cbBombCardCount)
{
    BYTE cbTmpCardData[MAX_COUNT];
    memcpy(cbTmpCardData, cbHandCardData, cbHandCardCount);

    SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

    cbBombCardCount = 0;

    if(cbHandCardCount < 2) return;

    if(cbTmpCardData[0] == 0x4F && cbTmpCardData[1] == 0x04E)
    {
        cbBombCardData[cbBombCardCount++] = cbTmpCardData[0];
        cbBombCardData[cbBombCardCount++] = cbTmpCardData[1];
    }

    for(BYTE i = 0; i < cbHandCardCount; i++)
    {
        //变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(4==cbSameCount)
		{
			cbBombCardData[cbBombCardCount++] = cbTmpCardData[i] ;
			cbBombCardData[cbBombCardCount++] = cbTmpCardData[i+1] ;
			cbBombCardData[cbBombCardCount++] = cbTmpCardData[i+2] ;
			cbBombCardData[cbBombCardCount++] = cbTmpCardData[i+3] ;
		}

		//设置索引
		i+=cbSameCount-1;
    }   
}

//分析出牌
VOID AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[12+1])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*12) ;
	BYTE cbTmpCardData[MAX_COUNT] ;
	//保留扑克，防止分析时改变扑克
	BYTE cbReserveCardData[MAX_COUNT] ;
	CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount) ;	
	SortCardList(cbReserveCardData, cbHandCardCount, ST_ORDER) ;
	CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

	//单牌类型
	for(BYTE i=0; i<cbHandCardCount; ++i)
	{
		BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount ;
		CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE ;
		CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbTmpCardData[i] ;
		CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1 ;
		CardTypeResult[CT_SINGLE].cbCardTypeCount++ ;	

		assert(CardTypeResult[CT_SINGLE].cbCardTypeCount<MAX_TYPE_COUNT) ;
	}

	//对牌类型
	{
		BYTE cbDoubleCardData[MAX_COUNT] ;
		BYTE cbDoubleCardcount=0; 
		GetAllDoubleCard(cbTmpCardData, cbHandCardCount, cbDoubleCardData, cbDoubleCardcount) ;
		for(BYTE i=0; i<cbDoubleCardcount; i+=2)
		{
			BYTE Index = CardTypeResult[CT_DOUBLE].cbCardTypeCount ;
			CardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE ;
			CardTypeResult[CT_DOUBLE].cbCardData[Index][0] = cbDoubleCardData[i] ;
			CardTypeResult[CT_DOUBLE].cbCardData[Index][1] = cbDoubleCardData[i+1] ;
			CardTypeResult[CT_DOUBLE].cbEachHandCardCount[Index] = 2 ;
			CardTypeResult[CT_DOUBLE].cbCardTypeCount++ ;	

			assert(CardTypeResult[CT_DOUBLE].cbCardTypeCount<MAX_TYPE_COUNT) ;
		}
	}

	//三条类型
	{
		BYTE cbThreeCardData[MAX_COUNT];
		BYTE cbThreeCardCount=0 ;
		GetAllThreeCard(cbTmpCardData, cbHandCardCount, cbThreeCardData, cbThreeCardCount) ;
		for(BYTE i=0; i<cbThreeCardCount; i+=3)
		{
			BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount ;
			CardTypeResult[CT_THREE].cbCardType = CT_THREE ;
			CardTypeResult[CT_THREE].cbCardData[Index][0] = cbThreeCardData[i] ;
			CardTypeResult[CT_THREE].cbCardData[Index][1] = cbThreeCardData[i+1] ;
			CardTypeResult[CT_THREE].cbCardData[Index][2] = cbThreeCardData[i+2] ;
			CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = 3 ;
			CardTypeResult[CT_THREE].cbCardTypeCount++ ;	

			assert(CardTypeResult[CT_THREE].cbCardTypeCount<MAX_TYPE_COUNT) ;
		}
	}

	//炸弹类型
	{
		BYTE cbFourCardData[MAX_COUNT];
		BYTE cbFourCardCount=0 ;
		if(cbHandCardCount>=2 && 0x4F==cbTmpCardData[0] && 0x4E==cbTmpCardData[1])
		{
			BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
			CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbTmpCardData[0] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbTmpCardData[1] ;
			CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 2 ;
			CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;	
			GetAllBombCard(cbTmpCardData+2, cbHandCardCount-2, cbFourCardData, cbFourCardCount) ;
		}
		else GetAllBombCard(cbTmpCardData, cbHandCardCount, cbFourCardData, cbFourCardCount) ;
		for (BYTE i=0; i<cbFourCardCount; i+=4)
		{
			BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
			CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbFourCardData[i] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbFourCardData[i+1] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][2] = cbFourCardData[i+2] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][3] = cbFourCardData[i+3] ;
			CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4 ;
			CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;	

			assert(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<MAX_TYPE_COUNT) ;
		}
	}
	//单连类型
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		BYTE cbFirstCard = 0 ;
		//去除2和王
		for(BYTE i=0 ; i<cbHandCardCount ; ++i)
		{
			if(GetCardLogicValue(cbTmpCardData[i])<15)
			{
				cbFirstCard = i ;
				break ;
			}
		}

		BYTE cbSingleLineCard[12] ;
		BYTE cbSingleLineCount=1 ;
		BYTE cbLeftCardCount = cbHandCardCount ;
		bool bFindSingleLine = true ;

		//连牌判断
		while (cbLeftCardCount>=5 && bFindSingleLine)
		{
			cbSingleLineCount=1 ;
			bFindSingleLine = false ;
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			cbSingleLineCard[cbSingleLineCount-1] = cbTmpCardData[cbFirstCard] ;
			for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
			{
				BYTE cbCardData=cbTmpCardData[i];

				//连续判断
				if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
				{
					cbLastCard = cbTmpCardData[i] ;
					//是否合法
					if(cbSingleLineCount<5) 
					{
						cbSingleLineCount = 1 ;
						cbSingleLineCard[cbSingleLineCount-1] = cbTmpCardData[i] ;
						continue ;
					}
					else break ;
				}
				//同牌判断
				else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
				{
					cbLastCard = cbCardData ;
					cbSingleLineCard[cbSingleLineCount] = cbCardData ;
					++cbSingleLineCount ;
				}
			}

			//保存数据
			if(cbSingleLineCount>=5)
			{
				BYTE Index ;
				//所有连牌
				BYTE cbStart=0 ;
				//从大到小
				while (cbSingleLineCount-cbStart>=5)
				{
					Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
					BYTE cbThisLineCount = cbSingleLineCount-cbStart ;
					CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard+cbStart, sizeof(BYTE)*(cbThisLineCount));
					CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbThisLineCount;
					CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;

					assert(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					cbStart++ ;
				}
				//从小到大
				cbStart=1 ;
				while (cbSingleLineCount-cbStart>=5)
				{
					Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
					BYTE cbThisLineCount = cbSingleLineCount-cbStart ;
					CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard, sizeof(BYTE)*(cbThisLineCount));
					CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbThisLineCount;
					CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;

					assert(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					cbStart++ ;
				}

				RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCardData, cbLeftCardCount) ;
				cbLeftCardCount -= cbSingleLineCount ;
				bFindSingleLine = true ;
			}
		}

	}

	//对连类型
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		//连牌判断
		BYTE cbFirstCard = 0 ;
		//去除2和王
		for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCardData[i])<15)	{cbFirstCard = i ; break ;}

		BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
		bool bFindDoubleLine = true ;
		BYTE cbDoubleLineCount = 0 ;
		BYTE cbDoubleLineCard[24] ;
		//开始判断
		while (cbLeftCardCount>=6 && bFindDoubleLine)
		{
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			BYTE cbSameCount = 1 ;
			cbDoubleLineCount = 0 ;
			bFindDoubleLine=false ;
			for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
			{
				//搜索同牌
				while (GetCardLogicValue(cbLastCard)==GetCardLogicValue(cbTmpCardData[i]) && i<cbLeftCardCount+cbFirstCard)
				{
					++cbSameCount;
					++i ;
				}

				BYTE cbLastDoubleCardValue ;
				if(cbDoubleLineCount>0) cbLastDoubleCardValue = GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) ;
				//重新开始
				if((cbSameCount<2 || (cbDoubleLineCount>0 && (cbLastDoubleCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
				{
					if(cbDoubleLineCount>=6) break ;
					//回退
					if(cbSameCount>=2) i-=cbSameCount ;
					cbLastCard = cbTmpCardData[i] ;
					cbDoubleLineCount = 0 ;
				}
				//保存数据
				else if(cbSameCount>=2)
				{
					cbDoubleLineCard[cbDoubleLineCount] = cbTmpCardData[i-cbSameCount] ;
					cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCardData[i-cbSameCount+1] ;
					cbDoubleLineCount += 2 ;

					//结尾判断
					if(i==(cbLeftCardCount+cbFirstCard-2))
						if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCardData[i]))==1 && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+1])))
						{
							cbDoubleLineCard[cbDoubleLineCount] = cbTmpCardData[i] ;
							cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCardData[i+1] ;
							cbDoubleLineCount += 2 ;
							break ;
						}

				}

				cbLastCard = cbTmpCardData[i] ;
				cbSameCount = 1 ;
			}

			//保存数据
			if(cbDoubleLineCount>=6)
			{
				BYTE Index ;

				Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount ;
				CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE ;
				CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], cbDoubleLineCard, sizeof(BYTE)*cbDoubleLineCount);
				CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = cbDoubleLineCount;
				CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++ ;

				assert(CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;

				RemoveCard(cbDoubleLineCard, cbDoubleLineCount, cbTmpCardData, cbFirstCard+cbLeftCardCount) ;				
				bFindDoubleLine=true ;
				cbLeftCardCount -= cbDoubleLineCount ;
			}
		}
	}

	//三连类型
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		//连牌判断
		BYTE cbFirstCard = 0 ;
		//去除2和王
		for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCardData[i])<15)	{cbFirstCard = i ; break ;}

		BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
		bool bFindThreeLine = true ;
		BYTE cbThreeLineCount = 0 ;
		BYTE cbThreeLineCard[20] ;
		//开始判断
		while (cbLeftCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			BYTE cbSameCount = 1 ;
			cbThreeLineCount = 0 ;
			bFindThreeLine = false ;
			for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
			{
				//搜索同牌
				while (GetCardLogicValue(cbLastCard)==GetCardLogicValue(cbTmpCardData[i]) && i<cbLeftCardCount+cbFirstCard)
				{
					++cbSameCount;
					++i ;
				}

				BYTE cbLastThreeCardValue ;
				if(cbThreeLineCount>0) cbLastThreeCardValue = GetCardLogicValue(cbThreeLineCard[cbThreeLineCount-1]) ;

				//重新开始
				if((cbSameCount<3 || (cbThreeLineCount>0&&(cbLastThreeCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
				{
					if(cbThreeLineCount>=6) break ;

					if(cbSameCount>=3) i-=cbSameCount ;
					cbLastCard = cbTmpCardData[i] ;
					cbThreeLineCount = 0 ;
				}
				//保存数据
				else if(cbSameCount>=3)
				{
					cbThreeLineCard[cbThreeLineCount] = cbTmpCardData[i-cbSameCount] ;
					cbThreeLineCard[cbThreeLineCount+1] = cbTmpCardData[i-cbSameCount+1] ;
					cbThreeLineCard[cbThreeLineCount+2] = cbTmpCardData[i-cbSameCount+2] ;
					cbThreeLineCount += 3 ;

					//结尾判断
					if(i==(cbLeftCardCount+cbFirstCard-3))
						if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCardData[i]))==1 && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+1])) && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+2])))
						{
							cbThreeLineCard[cbThreeLineCount] = cbTmpCardData[i] ;
							cbThreeLineCard[cbThreeLineCount+1] = cbTmpCardData[i+1] ;
							cbThreeLineCard[cbThreeLineCount+2] = cbTmpCardData[i+2] ;
							cbThreeLineCount += 3 ;
							break ;
						}

				}

				cbLastCard = cbTmpCardData[i] ;
				cbSameCount = 1 ;
			}

			//保存数据
			if(cbThreeLineCount>=6)
			{
				BYTE Index ;

				Index = CardTypeResult[CT_THREE_LINE].cbCardTypeCount ;
				CardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE ;
				CopyMemory(CardTypeResult[CT_THREE_LINE].cbCardData[Index], cbThreeLineCard, sizeof(BYTE)*cbThreeLineCount);
				CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[Index] = cbThreeLineCount;
				CardTypeResult[CT_THREE_LINE].cbCardTypeCount++ ;

				assert(CardTypeResult[CT_THREE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;

				RemoveCard(cbThreeLineCard, cbThreeLineCount, cbTmpCardData, cbFirstCard+cbLeftCardCount) ;
				bFindThreeLine=true ;
				cbLeftCardCount -= cbThreeLineCount ;
			}
		}

	}
	//三带一单
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		BYTE cbHandThreeCard[MAX_COUNT];
		BYTE cbHandThreeCount=0 ;

		//移除炸弹
		BYTE cbAllBomCardData[MAX_COUNT] ;
		BYTE cbAllBomCardCount=0 ;
		GetAllBombCard(cbTmpCardData, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
		RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCardData, cbHandCardCount) ;

		GetAllThreeCard(cbTmpCardData, cbHandCardCount-cbAllBomCardCount, cbHandThreeCard, cbHandThreeCount) ;

		{
			BYTE Index ;
			//去掉三条
			BYTE cbRemainCardData[MAX_COUNT] ;
			CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount-cbAllBomCardCount) ;
			BYTE cbRemainCardCount=cbHandCardCount-cbAllBomCardCount-cbHandThreeCount ;
			RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCardData, cbHandCardCount-cbAllBomCardCount) ;
			//三条带一张
			for(BYTE i=0; i<cbHandThreeCount; i+=3)
			{
				//三条带一张
				for(BYTE j=0; j<cbRemainCardCount; ++j)
				{
					Index = CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount ;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE ;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][0] = cbHandThreeCard[i] ;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][1] = cbHandThreeCard[i+1] ;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][2] = cbHandThreeCard[i+2] ;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][3] = cbRemainCardData[j] ;
					CardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[Index] = 4 ;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++ ;
				}			
			}
		}

		//三连带单
		BYTE cbLeftThreeCardCount=cbHandThreeCount ;
		bool bFindThreeLine=true ;
		BYTE cbLastIndex=0 ;
		if(GetCardLogicValue(cbHandThreeCard[0])==15) cbLastIndex=3 ;
		while (cbLeftThreeCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[cbLastIndex]);
			BYTE cbThreeLineCard[MAX_COUNT];
			BYTE cbThreeLineCardCount=3;
			cbThreeLineCard[0]=cbHandThreeCard[cbLastIndex];
			cbThreeLineCard[1]=cbHandThreeCard[cbLastIndex+1];
			cbThreeLineCard[2]=cbHandThreeCard[cbLastIndex+2];

			bFindThreeLine = false ;
			for(BYTE j=3+cbLastIndex; j<cbLeftThreeCardCount; j+=3)
			{
				//连续判断
				if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[j]))))
				{
					cbLastIndex = j ;
					if(cbLeftThreeCardCount-j>=6) bFindThreeLine = true ;

					break;
				}

				cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[j]);
				cbThreeLineCard[cbThreeLineCardCount]=cbHandThreeCard[j];
				cbThreeLineCard[cbThreeLineCardCount+1]=cbHandThreeCard[j+1];
				cbThreeLineCard[cbThreeLineCardCount+2]=cbHandThreeCard[j+2];
				cbThreeLineCardCount += 3;
			}
			if(cbThreeLineCardCount>3)
			{
				BYTE Index ;

				BYTE cbRemainCard[MAX_COUNT];
				BYTE cbRemainCardCount=cbHandCardCount-cbAllBomCardCount-cbHandThreeCount ;


				//移除三条（还应该移除炸弹王等）
				CopyMemory(cbRemainCard, cbTmpCardData, (cbHandCardCount-cbAllBomCardCount)*sizeof(BYTE));
				RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCard, cbHandCardCount-cbAllBomCardCount) ;

				for(BYTE start=0; start<cbThreeLineCardCount-3; start+=3)
				{
					//本顺数目
					BYTE cbThisTreeLineCardCount = cbThreeLineCardCount-start ;
					//单牌个数
					BYTE cbSingleCardCount=(cbThisTreeLineCardCount)/3;

					//单牌不够
					if(cbRemainCardCount<cbSingleCardCount) continue ;

					//单牌组合
					BYTE cbComCard[5];
					BYTE cbComResCard[254][5] ;
					BYTE cbComResLen=0 ;

					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
					for(BYTE i=0; i<cbComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount ;
						CardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE ;
						//保存三条
						CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index], cbThreeLineCard+start, sizeof(BYTE)*cbThisTreeLineCardCount);
						//保存单牌
						CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index]+cbThisTreeLineCardCount, cbComResCard[i], cbSingleCardCount) ;


						CardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[Index] = cbThisTreeLineCardCount+cbSingleCardCount ;
						CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++ ;

						assert(CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					}

				}

				//移除三连
				bFindThreeLine = true ;
				RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbHandThreeCard, cbLeftThreeCardCount) ;
				cbLeftThreeCardCount -= cbThreeLineCardCount ;
			}
		}
	}

	//三带一对
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		BYTE cbHandThreeCard[MAX_COUNT];
		BYTE cbHandThreeCount=0 ;
		BYTE cbRemainCarData[MAX_COUNT] ;
		BYTE cbRemainCardCount=0 ;

		//抽取三条
		GetAllThreeCard(cbTmpCardData, cbHandCardCount, cbHandThreeCard, cbHandThreeCount) ;

		//移除三条（还应该移除炸弹王等）
		CopyMemory(cbRemainCarData, cbTmpCardData, cbHandCardCount) ;
		RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCarData, cbHandCardCount) ;
		cbRemainCardCount = cbHandCardCount-cbHandThreeCount ;

		//抽取对牌
		BYTE cbAllDoubleCardData[MAX_COUNT] ;
		BYTE cbAllDoubleCardCount=0 ;
		GetAllDoubleCard(cbRemainCarData, cbRemainCardCount, cbAllDoubleCardData, cbAllDoubleCardCount) ;

		//三条带一对
		for(BYTE i=0; i<cbHandThreeCount; i+=3)
		{
			BYTE Index ;

			//三条带一张
			for(BYTE j=0; j<cbAllDoubleCardCount; j+=2)
			{
				Index = CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount ;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO ;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][0] = cbHandThreeCard[i] ;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][1] = cbHandThreeCard[i+1] ;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][2] = cbHandThreeCard[i+2] ;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][3] = cbAllDoubleCardData[j] ;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][4] = cbAllDoubleCardData[j+1] ;
				CardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[Index] = 5 ;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++ ;
			}	
		}

		//三连带对
		BYTE cbLeftThreeCardCount=cbHandThreeCount ;
		bool bFindThreeLine=true ;
		BYTE cbLastIndex=0 ;
		if(GetCardLogicValue(cbHandThreeCard[0])==15) cbLastIndex=3 ;
		while (cbLeftThreeCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[cbLastIndex]);
			BYTE cbThreeLineCard[MAX_COUNT];
			BYTE cbThreeLineCardCount=3;
			cbThreeLineCard[0]=cbHandThreeCard[cbLastIndex];
			cbThreeLineCard[1]=cbHandThreeCard[cbLastIndex+1];
			cbThreeLineCard[2]=cbHandThreeCard[cbLastIndex+2];

			bFindThreeLine=false ;
			for(BYTE j=3+cbLastIndex; j<cbLeftThreeCardCount; j+=3)
			{
				//连续判断
				if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[j]))))
				{
					cbLastIndex = j ;
					if(cbLeftThreeCardCount-j>=6) bFindThreeLine = true ;

					break;
				}

				cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[j]);
				cbThreeLineCard[cbThreeLineCardCount]=cbHandThreeCard[j];
				cbThreeLineCard[cbThreeLineCardCount+1]=cbHandThreeCard[j+1];
				cbThreeLineCard[cbThreeLineCardCount+2]=cbHandThreeCard[j+2];
				cbThreeLineCardCount += 3;
			}
			if(cbThreeLineCardCount>3)
			{
				BYTE Index ;

				for(BYTE start=0; start<cbThreeLineCardCount-3; start+=3)
				{
					//本顺数目
					BYTE cbThisTreeLineCardCount = cbThreeLineCardCount-start ;
					//对牌张数
					BYTE cbDoubleCardCount=((cbThisTreeLineCardCount)/3);

					//对牌不够
					if(cbRemainCardCount<cbDoubleCardCount) continue ;

					BYTE cbDoubleCardIndex[10]; //对牌下标
					for(BYTE i=0, j=0; i<cbAllDoubleCardCount; i+=2, ++j)
						cbDoubleCardIndex[j]=i ;

					//对牌组合
					BYTE cbComCard[5];
					BYTE cbComResCard[254][5] ;
					BYTE cbComResLen=0 ;

					//利用对牌的下标做组合，再根据下标提取出对牌
					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, cbDoubleCardCount, cbAllDoubleCardCount/2, cbDoubleCardCount);

					assert(cbComResLen<=254) ;

					for(BYTE i=0; i<cbComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount ;
						CardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO ;
						//保存三条
						CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index], cbThreeLineCard+start, sizeof(BYTE)*cbThisTreeLineCardCount);
						//保存对牌
						for(BYTE j=0, k=0; j<cbDoubleCardCount; ++j, k+=2)
						{
							CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][cbThisTreeLineCardCount+k] = cbAllDoubleCardData[cbComResCard[i][j]];
							CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][cbThisTreeLineCardCount+k+1] = cbAllDoubleCardData[cbComResCard[i][j]+1];
						}

						CardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[Index] = cbThisTreeLineCardCount+2*cbDoubleCardCount ;
						CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++ ;

						assert(CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount<MAX_TYPE_COUNT) ;
					}

				}
				//移除三连
				bFindThreeLine = true ;				
				RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbHandThreeCard, cbLeftThreeCardCount) ;
				cbLeftThreeCardCount -= cbThreeLineCardCount ;
			}
		}
	}
    return;
}

void AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbTurnCardData[], BYTE const cbTurnCardCount, tagOutCardTypeResult CardTypeResult[12+1])
{
    memset(CardTypeResult, 0, sizeof(CardTypeResult));

    if(cbHandCardCount < cbTurnCardCount) return;

    BYTE cbTmpCard[MAX_COUNT];
    memcpy(cbTmpCard, cbHandCardData, cbHandCardCount);
    SortCardList(cbTmpCard, cbHandCardCount, ST_ORDER);
    BYTE cbTurnCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
    
    if(cbTurnCardType == CT_ERROR)
        return;

    //对方出的不是炸弹
    if(cbTurnCardType != CT_MISSILE_CARD && cbTurnCardType != CT_BOMB_CARD)
    {
        //当有双王时
        if(cbHandCardCount >= 2 && 0x4F == cbTmpCard[0] && 0x4E == cbTmpCard[1])
        {
            BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
            CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
            CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbTmpCard[0];
            CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbTmpCard[1];

            CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 2;
            CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

            BYTE cbBombCardData[MAX_COUNT];
            BYTE cbBombCardCount = 0;
            GetAllBombCard(cbTmpCard + 2, cbHandCardCount - 2, cbBombCardData, cbBombCardCount);
            for(BYTE i = 0; i < cbBombCardCount/4; i++)
            {
                Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
                CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
                memcpy(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbBombCardData + 4*i, 4);
                CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4;
                CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

                assert(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount <= MAX_TYPE_COUNT);
            }
        }
        //炸弹类型
        else
		{
			BYTE cbBomCardData[MAX_COUNT];
			BYTE cbBomCardCount=0;
			GetAllBombCard(cbTmpCard, cbHandCardCount, cbBomCardData, cbBomCardCount) ;
			for(BYTE i=0; i<cbBomCardCount/4; ++i)
			{
				BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
				CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
				CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbBomCardData+4*i, 4) ;
				CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4;
				CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

				assert(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
			}
		}
    }
    switch(cbTurnCardType)
    {
        case CT_SINGLE:
        {
            for(BYTE i = 0; i < cbHandCardCount; ++i)
            {
                if(GetCardLogicValue(cbTmpCard[i]) > GetCardLogicValue(cbTurnCardData[0]))
                {
                    BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
                    CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE ;
					CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
					CardTypeResult[CT_SINGLE].cbCardTypeCount++ ;
                }
            }
            break;
        }
        case CT_DOUBLE:
        {
            for(BYTE i = 0; i < cbHandCardCount; i++)
            {
                BYTE cbSameCount = 1;
                BYTE cbLogicValue = GetCardLogicValue(cbTmpCard[i]);

                //搜索同牌
				for (BYTE j=i+1;j<cbHandCardCount;j++)
				{
					//获取扑克
					if (GetCardLogicValue(cbTmpCard[j])!=cbLogicValue) break;

					//设置变量
					cbSameCount++;
				}

                if(cbSameCount >= 2 && GetCardLogicValue(cbTmpCard[i]) > GetCardLogicValue(cbTurnCardData[0]))
                {
                    BYTE Index = CardTypeResult[CT_DOUBLE].cbCardTypeCount;
					CardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE ;
					CardTypeResult[CT_DOUBLE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_DOUBLE].cbCardData[Index][1] = cbTmpCard[i+1];
					CardTypeResult[CT_DOUBLE].cbEachHandCardCount[Index] = 2;
					CardTypeResult[CT_DOUBLE].cbCardTypeCount++ ;
                }
                i+=cbSameCount-1;
            }
            break;
        }
        case CT_THREE:
        {
            for(BYTE i = 0; i < cbHandCardCount; i++)
            {
                BYTE cbSameCount = 1;
                BYTE cbLogicValue = GetCardLogicValue(cbTmpCard[i]);

                for(BYTE j = i+1; j < cbHandCardCount; j++)
                {
                    if(GetCardLogicValue(cbTmpCard[j]) != cbLogicValue) break;
                    cbSameCount++;
                }

                if(cbSameCount >= 3 && GetCardLogicValue(cbTmpCard[i]) > GetCardLogicValue(cbTurnCardData[0]))
                {
                    BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount;
					CardTypeResult[CT_THREE].cbCardType = CT_THREE;
					CardTypeResult[CT_THREE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_THREE].cbCardData[Index][1] = cbTmpCard[i+1];
					CardTypeResult[CT_THREE].cbCardData[Index][2] = cbTmpCard[i+2];
					CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = 3;
					CardTypeResult[CT_THREE].cbCardTypeCount++;
                }
                i += cbSameCount - 1;
            }
            break;
        }
        case CT_SINGLE_LINE:
        {
            BYTE cbFirstCard = 0;
            for(BYTE i = 0; i < cbHandCardCount; i++) if(GetCardLogicValue(cbTmpCard[i]) < 15) {cbFirstCard = i; break;}

            BYTE cbSingleLineCard[12];
            BYTE cbSingleLineCount = 1;
            BYTE cbLeftCardCount = cbHandCardCount;
            bool bFindSingleLine = true;

            while(cbLeftCardCount >= cbTurnCardCount && bFindSingleLine)
            {
                cbSingleLineCount = 1;
                bFindSingleLine = false;
                BYTE cbLastCard = cbTmpCard[cbFirstCard];
                cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[cbFirstCard];
                for(BYTE i = cbFirstCard+1; i < cbLeftCardCount; i++)
                {
                    BYTE cbCardData = cbTmpCard[i];
                    //连续判断
					if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
					{
						cbLastCard = cbTmpCard[i] ;
						//是否合法
						if(cbSingleLineCount<cbTurnCardCount) 
						{
							cbSingleLineCount = 1 ;
							cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[i] ;
							continue ;
						}
						else break ;
					}
					//同牌判断
					else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
					{
						cbLastCard = cbCardData ;
						cbSingleLineCard[cbSingleLineCount] = cbCardData ;
						++cbSingleLineCount ;
					}
                }

                //保存数据
				if(cbSingleLineCount>=cbTurnCardCount && GetCardLogicValue(cbSingleLineCard[0])>GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE Index ;
					BYTE cbStart=0 ;
					//所有连牌
					while (GetCardLogicValue(cbSingleLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbSingleLineCount-cbStart)>=cbTurnCardCount))
					{
						Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
						CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbTurnCardCount;
						CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;
						cbStart++;

						assert(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
					}

					RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCard, cbLeftCardCount) ;
					cbLeftCardCount -= cbSingleLineCount ;
					bFindSingleLine = true ;
				}
            }

            break;
        }
    }
}


int main()
{

    BYTE cards[20] = {0x23,0x33,0x14,0x25,0x35,0x16,0x7,0x8,0x28,0x29,0x39,0x2A,0x1B,0x2C,0x3C,0x2,0x12};
    SortCardList(cards, 17, ST_ORDER);
    tagAnalyseResult AnalyseResult;
    AnalysebCardData(cards, 17, AnalyseResult);
    return 0;
}