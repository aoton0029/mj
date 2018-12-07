#include<stdio.h>
#include<stdlib.h>

int tehai[38];

/* *　国士無双のシャンテン数を返す */
int KokusiSyanten()
{
	int kokusi_toitu=0,syanten_kokusi=13,i;             
	//老頭牌
	for(i=1;i<30;i++){        
		if(i%10==1||i%10==9||i%20==1||i%20==9){	   
			if(tehai[i])
				syanten_kokusi--;
			if(tehai[i] >=2 && kokusi_toitu==0)
				kokusi_toitu=1;	
		}
	}             
	//字牌
	for(i=31;i<38;i++){      
		if(tehai[i]){	        
			syanten_kokusi--;
			if(tehai[i] >=2 && kokusi_toitu==0)
				kokusi_toitu=1;			            
		}
	}             
	//頭
	syanten_kokusi-= kokusi_toitu;             
	return syanten_kokusi;
}
/***************************************************************************************/
/* *　チートイツのシャンテン数を返す */
int TiitoituSyanten()
{
	int i=1,toitu=0,syurui=0,syanten_tiitoi;
	//トイツ数を数える
	for(;i<=37;i++){ 
		for(;!tehai[i];i++);
		if(i>=38) continue;
		syurui++;
		if(tehai[i] >=2)
		toitu++;
	}
	syanten_tiitoi=6-toitu;
	//４枚持ちを考慮
	if(syurui<7)
		syanten_tiitoi+= 7-syurui;
	return syanten_tiitoi;
}
/***************************************************************************************/
/* *　通常手のシャンテン数を返す */
void mentu_cut(int);//メンツ抜き関数
void taatu_cut(int);//ターツ抜き関数
//作業用グローバル変数
int mentu;		//メンツ数
int toitu;		//トイツ数
int kouho;		//ターツ数
int fuurosuu;		//フーロ数
int temp;		//シャンテン数（計算用）
int syanten_normal;	//シャンテン数（結果用）
/***************************************************************************************/
//入口（この関数だけを呼びだす）
int NormalSyanten()
{
	//初期化
	mentu=0;
	toitu=0;
	kouho=0;
	temp=0;
	syanten_normal=8;
	for(int i=1;i<38;i++)
	{
		//頭抜き出し
		if(2 <= tehai[i])
		{            
			toitu++;
			tehai[i] -= 2;
			mentu_cut(1);
			tehai[i] += 2;
			toitu--;
		}
	}
	mentu_cut(1);   //頭無しと仮定して計算
	return syanten_normal;	//最終的な結果
}
/***************************************************************************************/
//メンツ抜き出し
void mentu_cut(int i)
{
	for(;!tehai[i];i++);
	if(i>=38){taatu_cut(1);return;}//メンツを抜き終わったのでターツ抜きへ
	//コーツ
	if(tehai[i]>=3)
	{
		mentu++;
		tehai[i]-=3;
		mentu_cut(i);
		tehai[i]+=3;
		mentu--;
	}
	//シュンツ
	if(tehai[i+1]&&tehai[i+2]&&i<30)
	{
		mentu++;
		tehai[i]--,tehai[i+1]--,tehai[i+2]--;
		mentu_cut(i);
		tehai[i]++,tehai[i+1]++,tehai[i+2]++;
		mentu--;
	}
	//メンツ無しと仮定
	mentu_cut(i+1);
}
/***************************************************************************************/
//ターツ抜き出し
void taatu_cut(int i)
{													   
	for(;!tehai[i];i++);
	if(i>=38) //抜き出し終了
	{
		temp=8-mentu*2-kouho-toitu;
		if(temp<syanten_normal) { syanten_normal=temp; }
		return;
	}
	if(mentu+kouho<4)
	{            
		//トイツ
		if(tehai[i]==2)
		{
			kouho++;
			tehai[i]-=2;
			taatu_cut(i);
			tehai[i]+=2;
			kouho--;
		}
	             
		//ペンチャンorリャンメン
		if(tehai[i+1]&&i<30)
		{
			kouho++;
			tehai[i]--,tehai[i+1]--;
			taatu_cut(i);
			tehai[i]++,tehai[i+1]++;
			kouho--;
		}
	             
		//カンチャン
		if(tehai[i+2]&&i<30&&i%10<=8)
		{
			kouho++;
			tehai[i]--,tehai[i+2]--;
			taatu_cut(i);
			tehai[i]++,tehai[i+2]++;
			kouho--;
		}
	}
	taatu_cut(i+1);
}

int main(void){
	for(int i=0;i<14;i++){
		int t = rand()%37; 
		tehai[t] += 1;
	}
	for(int i=0;i<38;i++){
		printf("%d ",tehai[i]);
	}
	printf("\n");
	int kokushi = KokusiSyanten();
	printf("%d ",kokushi);
	int tiitoi = TiitoituSyanten();
	printf("%d ",tiitoi);
	int normal = NormalSyanten();
	printf("%d ",normal);
	printf("\n");
}
