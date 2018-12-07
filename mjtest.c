#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<memory.h>
#include<math.h>
#include <stdbool.h>
/*
#define M1 1
#define M2 2
#define M3 3
#define M4 4
#define M5 5
#define M6 6
#define M7 7
#define M8 8
#define M9 9

#define P1 11
#define P2 12
#define P3 13
#define P4 14
#define P5 15
#define P6 16
#define P7 17
#define P8 18
#define P9 19

#define S1 21
#define S2 22
#define S3 23
#define S4 24
#define S5 25
#define S6 26
#define S7 27
#define S8 28
#define S9 29

#define TON 31
#define NAN 32
#define SHA 33
#define PE 34
#define HAKU 35
#define HATU 36
#define CHUN 37
*/
#define FUTEI 20

//int handsample[14]={M1,M9,P1,P9,S1,S9,HAKU,HATU,CHUN,TON,NAN,SHA,PE,PE};
int hand[14];//手牌
int kind[38];//牌ごとに何枚あるか
int tmp[38];//和了型チェック用にkindをコピー
int tehai[38];
int tile[38] = { 0,1,0,0,0,0,0,0,0,1,
	     	 0,1,0,0,0,0,0,0,0,1,
	     	 0,1,0,0,0,0,0,0,0,1,
	     	 1,1,1,1,
	     	 1,1,1};
int head;//雀頭
int handdiv[4][3];
int handsort[14];
int Han,Fu,Point;
int handdivsum[4];
int dora;
int Bazoro;
int PointKo,PointOya;
int agarihai;
int syantensuu,tiitoisyantensuu;
bool ron;//ロンあがりならTRUEになるフラグ

enum Agari{Tsumo,Ron};
enum Bakaze{Ton, Nan, Sha, Pe};
enum Machi{Tanki,Penchan,Kanchan,Ryanmen,Shanpon,Nobetan};
enum Mentsu{PON=0,CHII,KANTU,ANKAN,MINKAN,KOTU,ANKO,MINKO,SYUNTU,TOITU};

struct Ba{
	int yama[136];
	int yamahai[70];
	int wanpai[14];
	enum Bakaze bakaze;
	int Dora;
}ba;

struct HandState{
	int hands[38];
	int tehai[14];
	int kawa[30];
	enum Agari agari;
	enum Machi machi;
	bool Oya;
	bool isMenzen;//面前か
	bool isHeadYakuhai;//雀頭が役牌か
	bool isRyanmen;//両面待ちか
	bool isReach;//リーチしてるか
	int fuuro[20];
	int fuuronum;//副露の数
/*
    [0+4*n]　フーロの種類（0＝フーロなし、1＝ポン、2＝チー、3＝アンカン、4＝ミンカン　5＝カカン）
    [1+4*n]　そのフーロメンツのうち最も小さい牌番号
    [2+4*n]　そのフーロメンツのうち鳴いた牌の牌番号
    [3+4*n]　その人から相対的に見た、鳴いた他家の番号。1＝下家、2＝対面、3＝上家
*/
	//役
	bool Ippatsu;		
	bool Menzentsumo;
	bool Yakuhai;	
	bool Tanyao;	
	bool Pinfu;		
	bool Ipeko;		
	bool Haitei;	
	bool Hotei;		
	bool Rinshan;	
	bool Tyankan;	
	bool DoubleReach;	
	bool SanshikiS;		
	bool SanshikiK;		
	bool Sananko;		
	bool Ikkikantsu;	
	bool chitoi;		
	bool Toitoi;		
	bool Tyanta;		
	bool Sankantsu;		
	bool Ryanpeko;		
	bool kokushi;
}handstate,playerhand[4];

struct MentsuStatus{
	int handdivsum;
	int handdiv[4];
	bool isFuro;
	bool jikaze;
	enum Mentsu mentsuSKK;	
};

struct MentsuStatus mentsu[4];


/*両面か⇒雀頭が役牌ではないか⇒両面待ちか⇒順子のみか⇒平和*/
/*役
１翻：*リーチ,*一発,*面前自摸,*役牌,*タンヤオ,平和,*一盃口,海底撈月,河底撈月,嶺上開花,槍槓
２翻：ダブリー,*三色順刻,*三暗刻,*一気通貫,*七対子,*対々和,*チャンタ,三槓子
３翻：二盃口,純チャン,混一色
４翻：小三元,混老頭
６翻：清一色
役満：
*/




int Sort(int number[],int total){
	int t,i,j; 
	for (i=0; i<total; ++i) {
	    for (j=i+1; j<total; ++j) {
		if (number[i] > number[j]) {
		  t =  number[i];
		  number[i] = number[j];
		  number[j] = t;
		}
	    }
	}
}
/*ゲーム部*/
void genelate_yama(int yama[])
{
	int i,j=0,k=0;
	//元の山を生成
	for(i=1;i<38;i++){
		if(i%10==0) continue;
		while(j++<4) yama[k++]=i;
		j=0;
	} 
	//山をシャッフル
	i=j=k=0;
	for(i=135;i>0;i--){
		j=rand()%i;
		k=yama[i];
		yama[i]=yama[j];
		yama[j]=k;
	}
	
	for(int i=0;i<136;i++){
		printf("%d ",ba.yama[i]);
	}
	
	//0~14:王牌,15~27,28~40,41~53,54~66:手牌,67~136:山
	for(int i=0;i<14;i++){
		ba.wanpai[i]=ba.yama[i];
	}
	for(int j=0;j<4;j++){
		for(int i=0;i<13;i++){
			playerhand[j].tehai[i]=ba.yama[15+13*j+i];
		}
	}
	for(int i=0;i<70;i++){
		ba.yamahai[i]=ba.yama[67+i];
	}
	
	printf("\n");
	printf("王牌：");
	for(int i=0;i<14;i++){
		printf("%d ",ba.wanpai[i]);
	}
	printf("\n");
	for(int j=0;j<4;j++){
		printf("手牌：");
		for(int i=0;i<13;i++){
			printf("%d ",playerhand[j].tehai[i]);
		}
		printf("\n");
	}
	printf("\n");
	printf("山牌：");
	for(int i=0;i<70;i++){
		printf("%d ",ba.yamahai[i]);
	}
	printf("\n");
	
	ba.Dora=ba.wanpai[rand() % 10 + 1];
	if(ba.Dora<30){
		if((ba.Dora+1)%10!=0){
			dora = ba.Dora+1;
		}else {
			dora = ba.Dora-8;
		}
	}else {
		if(ba.Dora==34){dora=31;}
		else if(ba.Dora==37){dora=35;}
		else {dora=ba.Dora;}
	}
	printf("ドラ：%d",dora);
	printf("\n");
	for(int i=0;i<4;i++){
		Sort(playerhand[i].tehai,13);
	}
}

void GameManager(){
	genelate_yama(ba.yama);
	
	
	
}

//シャンテン数
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
	for(int i=0;i<38;i++){
		tehai[i]=kind[i];
	}
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
int Makefuuro(int syurui,int sutehai,int playernum,int min){
/*
    [0+4*n]　フーロの種類（0＝フーロなし、1＝ポン、2＝チー、3＝アンカン、4＝ミンカン　5＝カカン）
    [1+4*n]　そのフーロメンツのうち最も小さい牌番号
    [2+4*n]　そのフーロメンツのうち鳴いた牌の牌番号
    [3+4*n]　その人から相対的に見た、鳴いた他家の番号。1＝下家、2＝対面、3＝上家
*/
	playerhand[playernum].fuuro[0+4*playerhand[playernum].fuuronum]=syurui;
	playerhand[playernum].fuuro[1+4*playerhand[playernum].fuuronum]=min;
	playerhand[playernum].fuuro[2+4*playerhand[playernum].fuuronum]=sutehai;
	playerhand[playernum].fuuro[3+4*playerhand[playernum].fuuronum]=playernum;
	
	playerhand[playernum].tehai[13]=sutehai;
	
}
bool FuuroCheck(int sutehai,int tehaitmp[],int fuuroplayernum,int playernum){
	bool canPON=false,canKAN=false,canCHII=false;
	int min;
	printf("プレイヤー%d：",fuuroplayernum+1);
	for(int i=0;i<14;i++){
		printf("%d ",tehaitmp[i]);
	}
	printf("\n");
	for(int i=0;i<14;i++){
		if((sutehai==tehaitmp[i])&&(sutehai==tehaitmp[i+1])){
			canPON=true;
			min =tehaitmp[i];
		}
		if((sutehai==tehaitmp[i])&&(sutehai==tehaitmp[i+1])&&(sutehai==tehaitmp[i+2])){
			canKAN=true;
			min = tehaitmp[i];
		}
		if(fuuroplayernum==playernum+1){
			if((sutehai<30)&&(sutehai+1==tehaitmp[i])&&(sutehai+2==tehaitmp[i+1])){//チー牌が一番小さいとき
				canCHII=true;
				min = sutehai;
			}else if((sutehai<30)&&(sutehai-1==tehaitmp[i+1])&&(sutehai-2==tehaitmp[i])){//チー牌が一番大きいとき
				canCHII=true;
				min = tehaitmp[i];
				
			}
		}
	}
	
	int x;
	if(canKAN==true){
		printf("%dをカンしますか y1/n0",sutehai);
		if(fuuroplayernum==0){scanf("%d",&x);}else {x=0;}
		if(x==1){
			Makefuuro(3,sutehai,fuuroplayernum,min);
			playerhand[fuuroplayernum].fuuronum++;
			return true;
		}else {
			canKAN=false;
		}
	}
	if(canPON==true){
		printf("%dをポンしますか y/n",sutehai);
		if(fuuroplayernum==0){scanf("%d",&x);}else {x=0;}
		if(x==1){
			Makefuuro(1,sutehai,fuuroplayernum,min);
			playerhand[fuuroplayernum].fuuronum++;
			return true;
		}else {
			canPON=false;
		}
	}
	if(canCHII==true){
		printf("%dをチーしますか y/n",sutehai);
		if(fuuroplayernum==0){scanf("%d",&x);}else {x=0;}
		if(x==1){
			Makefuuro(2,sutehai,fuuroplayernum,min);
			playerhand[fuuroplayernum].fuuronum++;
			return true;
		}else {
			canCHII=false;
		}
	}
	
	if((canPON==false)&&(canKAN==false)&&(canCHII==false)){return false;}
}

	
/*点数計算*/
int Init(){
	Han=0;
	Fu=0;
	Bazoro=0;
	PointKo=0;
	PointOya=0;
	
	ba.bakaze = Ton;
	
	handstate.chitoi=false;
	handstate.isReach=true;
	handstate.Oya=false;
	handstate.isMenzen=true;
	handstate.agari = Tsumo; 
	
	for(int i=0;i<4;i++){
		mentsu[i].isFuro=false;
		mentsu[i].jikaze=false;
	}
}

int Kokusi_Check(){
	int count=0;
	for(int i=1;i<30;i+=10){
		if((kind[i]==1)||(kind[i]==2)){
			count++;//1,11,21		
		}
		if((kind[i+8]==1)||(kind[i+8]==2)){
			count++;//9,19,29
		}
	}
	for(int i=31;i<38;i++){
		if((kind[i]==1)||(kind[i]==2)){
			count++;//31~37
		}
	}
	if(count==13) {
		printf("国士無双\n");
		handstate.kokushi==true;
	}
}

int Chitoi_Check(){
	int count=0;
	for(int i=0;i<38;i++){
		if(kind[i]==2) count++;
	}
	if(count==7) {
		handstate.chitoi=true;
		printf("七対子\n");	
	}
}

int Kokushi_Judge(){
	Han=13;
	Fu=0;
}

int Chitoi_Judge(){
	Han += 2;	
	Fu=25;
}

/*
int Machi_Check(){
	if(agarihai==head){
		headstate.machi=Tanki;
	}
	if(kind[agarihai]>=3){
		headstate.machi=Shanpon;
	}
}
*/

int Hora_Check(){
	//tmpにコピー
	
	printf("各牌枚数kind ： ");
	for(int k=0;k<38;k++){
		tmp[k]=kind[k];
		printf("%d ",tmp[k]);
	}
	
	printf("\n");
	int divnumber=0;
	//雀頭、和了型かを調べる、和了型ならアガリ
	for(int i=0;i<38;i++){
		if(tmp[i]>=2){//雀頭を決める
			tmp[i] -= 2;
			head = i;
			//printf("%d\n",head);
			for(int j=0;j<38;j++){
				if(tmp[j]>=3){
					tmp[j] -= 3;
					handdiv[divnumber][0]=j;
					handdiv[divnumber][1]=j;
					handdiv[divnumber][2]=j;
					mentsu[divnumber].handdivsum = j + 10*j + 100*j;
					mentsu[divnumber].mentsuSKK = KOTU;
					mentsu[divnumber].handdiv[0]=j;
					mentsu[divnumber].handdiv[1]=j;
					mentsu[divnumber].handdiv[2]=j;
					divnumber++;
				}
				if((((tmp[j]>0)&&(tmp[j+1]>0)&&(tmp[j+2]>0)))&&(j<30)){
					tmp[j] -=1;	
					tmp[j+1] -=1;
					tmp[j+2] -=1;
					handdiv[divnumber][0]=j;
					handdiv[divnumber][1]=j+1;
					handdiv[divnumber][2]=j+2;
					mentsu[divnumber].handdivsum = j*100 + (j+1)*10 + j+2;
					mentsu[divnumber].mentsuSKK = SYUNTU;
					mentsu[divnumber].handdiv[0]=j;
					mentsu[divnumber].handdiv[1]=j+1;
					mentsu[divnumber].handdiv[2]=j+2;
					divnumber++;
					if((tmp[j]!=0)&&(tmp[j+1]!=0)&&(tmp[j+2]!=0)) j-=1;
				}
			}
			//printf("\n");
			int Flag=0;
			while(Flag=0){
				for(int i=0;i<3;i++){
					for(int j=0;j<3;j++){
						if(handdiv[i][j]==0){
							Flag=1;
						}else {
							//count++;
						}
					}
				}
			}
		}
		//tmpを初期化
		for(int k=0;k<38;k++){
			tmp[k]=kind[k];
		}
		divnumber=0;
	}
}

int Yaku_Judge(){
	int count;
	//門前チェック
	for(int i=0;i<4;i++){
		if(mentsu[i].isFuro==false){
			handstate.isMenzen=true;
		}
	}
	//ドラ
	Han+=kind[dora];
	printf("ドラ%d\n",kind[dora]);
	
	//リーチ
	if(handstate.isReach==true){
		printf("リーチ\n");
		Han+=1;
	}
	//一発


	//門前自摸
	if((handstate.isMenzen==true)&&(handstate.agari==Tsumo)){
		printf("門前ツモ\n");
		Han+=1;
	}
	
	//一気通貫：1~9,11~19,21~29が連続しているとき 鳴くと-1翻
	for(int j=0;j<30;j+=10){
		for(int i=1+j;i<=9+j;i++){
			if(kind[i]<1) break;
			if(i==9+j) {
				printf("一気通貫\n"); 
				Han += 2;
			}
		}
	}
	
	count = 0;
	//タンヤオ：1,9,11,19,21,29,31,32,33,34,35,36,37が0のとき
	for(int i=1;i<30;i+=10){
		if((kind[i]==0)&&(kind[i+8]==0)){
			count+=1;
		}
	}
	for(int i=31;i<38;i++){
		if(kind[i]==0){
			count+=1;
		}
	}
	if(count==10){
		printf("タンヤオ\n");
		Han+=1;
	}
	
	//役牌：35,36,37が刻子のとき
	if(kind[35]>=3) {
		printf("役牌\n"); 
		Han+=1;
	}
	if(kind[36]>=3) {
		printf("役牌\n"); 
		Han+=1;
	}
	if(kind[37]>=3) {
		printf("役牌\n"); 
		Han+=1;
	}	
	
	//風牌：31,32,33,34の刻子が自風牌のとき
	if((kind[31]>=3)&&(ba.bakaze==Ton)){
		printf("風牌\n");
		Han+=1;
	}
	if((kind[32]>=3)&&(ba.bakaze==Nan)){
		printf("風牌\n");
		Han+=1;
	}
	if((kind[33]>=3)&&(ba.bakaze==Sha)){
		printf("風牌\n");
		Han+=1;
	}
	if((kind[34]>=3)&&(ba.bakaze==Pe)){
		printf("風牌\n");
		Han+=1;
	}

	//一盃口：同じ順子が2つあるとき
	for(int i=0;i<4;i++){
		for(int j=i-1;j>=0;j--){
			if((mentsu[i].mentsuSKK==SYUNTU)&&(mentsu[j].mentsuSKK==SYUNTU)&&(mentsu[i].handdivsum==mentsu[j].handdivsum)){
				printf("%d %d 一盃口\n",mentsu[i].handdivsum,mentsu[j].handdivsum);
			}
		}
	}

	//三色同順：鳴いて-1翻、1 2 3,11 12 13,21 22 23
	for(int i=1;i<=7;i++){
		if((kind[i]>0)&&(kind[i+10]>0)&&(kind[i+20]>0)){
			if((kind[i+1]>0)&&(kind[i+10+1]>0)&&(kind[i+20+1]>0)){
				if((kind[i+2]>0)&&(kind[i+10+2]>0)&&(kind[i+20+2]>0)){
					printf("三色同順\n");
					Han+=2;
				}	
			}	
		}			
	}
	
	//三色同刻：鳴いて-1翻
	for(int i=1;i<9;i++){
		if((kind[i]>=3)&&(kind[i+10]>=3)&&(kind[i+20]>=3)){
			printf("三色同刻\n");
			Han+=2;
		}
	}

	//対々和：刻子が4つ
	count=0;
	for(int i=0;i<38;i++){
		if(kind[i]>=3){
			count++;
		}
	}
	if(count==4) {
		printf("対々和\n"); 
		Han+=2;
	}

	count=0;
	//三暗刻
	for(int i=0;i<4;i++){
		if((mentsu[i].isFuro==false)&&(mentsu[i].mentsuSKK==KOTU)){
			count+=1;
		}
	}
	if(count>=3){
		printf("三暗刻");
		Han+=2;
	}

	count=0;
	//チャンタ　19字牌　鳴いて-1翻
	for(int i=0;i<4;i++){
		if((mentsu[i].handdiv[0]==1)||(mentsu[i].handdiv[2]==9)||(mentsu[i].handdiv[0]>30)){
			count += 1; 
		}
	}
	if((head>30)&&(count==4)){
		printf("チャンタ");
		Han+=2;
	}

	//二盃口

	//純チャンタ

	//混一色

	//小三元：頭と2面子を白發中、

	//混老頭

	Kokusi_Check();

	Chitoi_Check();	

}

int Han_Judge(){
	
}

int Fu_Judge(){
	//基本符２０符+和了(門前ロン１０符自摸２符)+各面子+雀頭(役牌２符)+待ち(ペンカンチャン単騎ノベタン２符)
	//ピンフツモ２０符、七対子２５符
	//if((handstate.pinfu==true)&&(handstate.menzentsumo==true)) Fu=20;
	
	Fu +=20;//副底
	//if(handstate.isMenzen==true){}

	for(int i=0;i<4;i++){
		int j = mentsu[i].mentsuSKK;
		switch(j){
			case SYUNTU:
				Fu+=0;
				break;
			case KOTU:
				if(((mentsu[i].handdiv[0]>1)&&(mentsu[i].handdiv[0]<9))
						||((mentsu[i].handdiv[0]>11)&&(mentsu[i].handdiv[0]<19))
								||((mentsu[i].handdiv[0]>21)&&(mentsu[i].handdiv[0]<29))){
					if(mentsu[i].isFuro==true){
						Fu += 2;//中張牌明刻
					}else{
						Fu += 4;//中張牌暗刻
					}		
				}else {
					if(mentsu[i].isFuro==true){
						Fu += 4;//么九牌明刻
					}else{
						Fu += 8;//么九牌暗刻
					}	
				}
				break;
			case KANTU:
				if(((mentsu[i].handdiv[0]>1)&&(mentsu[i].handdiv[0]<9))
						||((mentsu[i].handdiv[0]>11)&&(mentsu[i].handdiv[0]<19))
								||((mentsu[i].handdiv[0]>21)&&(mentsu[i].handdiv[0]<29))){
					if(mentsu[i].isFuro==true){
						Fu += 8;//中張牌明槓
					}else{
						Fu += 16;//中張牌暗槓
					}		
				}else {
					if(mentsu[i].isFuro==true){
						Fu += 16;
					}else{
						Fu += 32;
					}	
				}
				break;
		}
		printf("%d\n",Fu);
	}
	
	//頭
	if(head>30) {Fu+=2;}
	
	//待ち
	if((handstate.machi==Penchan)||(handstate.machi==Kanchan)
			||(handstate.machi==Tanki)||(handstate.machi==Nobetan)){
		Fu+=2;
	}
	
	//切り上げ
	double Fukari = (double)Fu;
	if(Fu%10!=0){
		double x = Fukari / 10; 
		//x = ceil(x); 
		x += 0.9;
		Fu = (int)x; 
		Fu = Fu * 10;
	}
	
}
//切り上げ
int Ceil(int point){
	int result;
	result = point / 100;
	result +=1;
	result = result * 100; 
	return result;
}

int Point_Judge(){
//ロン
	if((Han==5)||((Han==4)&&(Fu>=40))||((Han==3)&&(Fu>=70))){Point=8000;}
	else if((Han==6)||(Han==7)){Point=12000;}
	else if((Han==8)||(Han==9)||(Han==10)){Point=16000;}
	else if((Han==11)||(Han==12)){Point=24000;}
	else if(Han==13){Point=32000;}
	else {
		int jou=1;
		Bazoro = Han + 2;
		for(int i=0;i<Bazoro;i++){
			jou = jou * 2; 
		}
		Point = Fu * 4 * jou;  
		printf("%d\n",Point);
	}
	
	if(handstate.Oya==true){
		Point = Point * 1.5;
	}
	
	//切り上げ
	if(Point%100!=0){
		Point = Ceil(Point);
	} 
	printf("%d\n",Point);
//ツモ
	if(handstate.agari==Tsumo){
		if(handstate.Oya==true){
			if(Point%3!=0){
				Point = Point / 3;
				PointKo = Ceil(Point);
			}else{
				PointKo = Point / 3;
			} 
		}else{
			if(Point%4!=0){
				PointKo = Point / 4;
				PointOya = PointKo * 2;
				if(PointKo%100!=0){
					PointKo = Ceil(PointKo);
				}
				if(PointOya%100!=0){
					PointOya = Ceil(PointOya);
				}
			}else{
				PointKo = Point / 4;
				PointOya = PointKo * 2;
				if(PointKo%100!=0){
					PointKo = Ceil(PointKo);
				}
				if(PointOya%100!=0){
					PointOya = Ceil(PointOya);
				}
			}
		}
	}
}

int Result(){
	for(int i=0;i<4;i++){
		printf("面子 %d ： %d %d %d    ",i,handdiv[i][0],handdiv[i][1],handdiv[i][2]);
		printf("%d\n",mentsu[i].mentsuSKK);
	}
	
	handsort[0]=head;
	handsort[1]=head;
	int j=0;
	for(int i=2;i<14;i+=3){
		handsort[i]=handdiv[j][0];
		handsort[i+1]=handdiv[j][1];
		handsort[i+2]=handdiv[j][2];
		j++;
	}
	printf("理牌後 ： ");
	for(int i=0;i<14;i++){
		printf("%d ",handsort[i]);
	}
	printf("\n");
	if(handstate.agari==Ron){
		printf("ロン　%d翻　%d符　%d点\n",Han,Fu,Point);
	}
	if(handstate.agari==Tsumo){
		if(handstate.Oya==true){printf("親");}else{printf("子");}
		printf("ツモ　%d翻　%d符　　",Han,Fu);
		printf("親：%d点　子：%d点\n",PointOya,PointKo);
	}
}

int TensuuKeisan(){
	/*
	Init();
	int d;
	printf("萬子:1~9、筒子:11~19、索子:21~29、東南西北:31,32,33,34、三元牌:白35,發36,中37\n");
	for(int i=0;i<14;i++){
		scanf("%d",&hand[i]);
	}
	
	printf("上がり牌\n");
	scanf("%d",&agarihai);
	printf("手牌hand ： ");
	for(int i=0;i<14;i++){
		printf("%d ",hand[i]);
	}
	printf("\n");
	printf("親=>0、子=>1\n");
	scanf("%d",&d);
	if(d==0){
		handstate.Oya=true;
	}else if(d==1){
		handstate.Oya=false;
	}
	printf("リーチしてる？yes=>0、no=>1\n");
	scanf("%d",&d);
	if(d==0){
		handstate.isReach=true;
	}else if(d==1){
		handstate.isReach=false;
	}
	printf("ドラは？");
	scanf("%d",&dora);
	printf("ツモ=>0、ロン=>1\n");
	scanf("%d",&d);
	if(d==0){
		handstate.agari=Tsumo;
	}else if(d==1){
		handstate.agari=Ron;
	}
	
	*/
	//手牌の種ごとの枚数を数える
	for(int i=0;i<14;i++){
		int j = hand[i];
		kind[j] += 1;
	}
	
	printf("\n");
	Hora_Check();
	Yaku_Judge();
	if(handstate.kokushi==true){
		Kokushi_Judge();
	}else if(handstate.chitoi==true){
		Chitoi_Judge();
	}else{
		Han_Judge();
		Fu_Judge();
	}
	Point_Judge();

	Result();
}

int Flag=0,sute,fuuroplayer;

int TsumoSute(int playernum){
	Sort(playerhand[playernum].tehai,14);
	for(int i=0;i<14;i++){
		printf("%d ",playerhand[playernum].tehai[i]);
	}
	printf("\n");
	//手牌の種ごとの枚数を数える
	for(int i=0;i<14;i++){
		int j = playerhand[playernum].tehai[i];
		kind[j] += 1;
	}
	//Hora_Check();//和了かチェック
	//TensuuKeisan();
	syantensuu=NormalSyanten();
	tiitoisyantensuu=TiitoituSyanten();
	printf("シャンテン数：%d 七対子シャンテン数：%d\n",syantensuu,tiitoisyantensuu);
	//シャンテン数が0のとき副露していなければリーチができる
	if((syantensuu==0)||(tiitoisyantensuu==0)){
		int x; 
		printf("リーチする？y1/n0");
		scanf("%d",&x);
		if(x==1){
			playerhand[0].isReach==true;
		}
	}
	//捨てる牌を決める
	if(playernum==0){
		if(playerhand[0].isReach==false){
			printf("プレイヤー%d 何番目の牌を捨てる\n",playernum+1);
			scanf("%d",&sute);
		}else if(playerhand[0].isReach==true){
			sute=14;
		}
	}else {
		//CPU_AI();
		sute = rand()%10 + 1;
	}
	printf("捨て牌：%d\n",playerhand[playernum].tehai[sute-1]);
	//プレイヤーの捨てた牌が和了or副露牌かチェック
	//副露牌ならfuuro[]にいれて、手牌に追加し、手牌を捨てるステートに移行する
	bool fuurocheck = false;
	int i=playernum+1;
	while(i!=playernum){
			fuurocheck = FuuroCheck(playerhand[playernum].tehai[sute-1],playerhand[i].tehai,i,playernum);
			if(fuurocheck==true){
				//printf("副露プレイヤー%d\n",i+1);
				Flag =1;
				return i;
				break;
			}
		i++;
		if(i>3){i=0;}
	}
	if(fuurocheck==false){
		Flag=0;
		return playernum;
	}
}

void PlayerTehaiPrint(int j){
	for(int i=0;i<14;i++){
		printf("%d  ",playerhand[j-1].tehai[i]);
	}
}

void PlayerKawaPrint(int j){
	for(int i=0;i<30;i++){
		if(playerhand[j-1].kawa[i]!=0){
			printf("%d ",playerhand[j-1].kawa[i]);
		}
	}
	/*
	int k=0;
	while(k<5){
		for(int i=1;i<=6;i++){
			printf("%d ",playerhand[j-1].kawa[i+k*6]);
		}
		printf("\n");
		k++;
	}
	*/	
}

int end=0;
int main(void){
	GameManager();
	//TensuuKeisan();
	int k=1,playernum=0,jun=0;
	while(jun<70){
		while(playernum<4){
			while(!end){
			switch(Flag){
				case 1 :
				printf("case 1 プレイヤー %d\n",playernum+1);
					playernum = TsumoSute(playernum);
					//playerhand[fuuroplayer].tehai[13]=playerhand[playernum].tehai[sute-1];
					if(Flag!=1){end=1;}
					break;
				default :
				printf("case default プレイヤー %d\n",playernum+1);
					playerhand[playernum].tehai[13]=ba.yamahai[jun];
					playernum = TsumoSute(playernum);
					if(Flag!=1){end=1;}
					break;
				}
			}
			
			//もし和了なし副露なしor副露しないなら捨牌を河配列に格納する
			playerhand[playernum].kawa[k]=playerhand[playernum].tehai[sute-1];
			playerhand[playernum].tehai[sute-1]=40;
			Sort(playerhand[playernum].tehai,14);
			
			/*
			//確認用
			printf("\n");
			printf("プレイヤー%d手牌：",playernum+1);
			for(int i=0;i<14;i++){
				printf("%d ",playerhand[playernum].tehai[i]);
			}
			printf("\n");
			printf("プレイヤー%d河：",playernum+1);
			for(int i=0;i<30;i++){
				printf("%d ",playerhand[playernum].kawa[i]);
			}
			*/
			printf("\n");
			for(int i=0;i<38;i++){
				tmp[i]=0;
				kind[i]=0;
			}
			printf("-------------------------------------------------------\n");
			Flag=0;
			end=0;
			jun++;
			playernum++;
		}
		printf("*****************************************************\n\n");
		printf("プレイヤー３：");
		printf("河：");PlayerKawaPrint(3);printf("\n");
		printf("プレイヤー２：");
		printf("河:");PlayerKawaPrint(2);printf("\n");
		printf("プレイヤー４：");
		printf("河：");PlayerKawaPrint(4);printf("\n");
		printf("プレイヤー１：");
		printf("河：");PlayerKawaPrint(1);printf("\n");
		printf("	     1  2  3  4  5  6  7  8  9  10  11  12  13  14");printf("\n");
		printf("プレイヤー１:");PlayerTehaiPrint(1);printf("\n");
		printf("******************************************************\n\n");
		playernum=0;
		k++;
	}
	
}

/*
gameobject配列を用意し、牌をすべて入れる
HaiSet[38]
yama[146] = {1~38の４セット}
Transform.HaiSet[hand[i]]

＊山牌を用意（最後の１４牌が王牌）
＊山牌から１つ手牌を最後尾に加える
＊手牌の牌を１つ抜き出し捨牌に加える
＊手牌を並べ替え
＊シャンテン数チェック or △和了牌 or ＊副露牌
捨て牌がフーロ牌か和了牌であるかチェックする
→フーロ牌のとき、副露配列に格納してplayernumをそのプレイヤーに変える
→和了牌のとき、点数計算
＊シャンテン数の計算→聴牌かどうか→聴牌のときリーチするかしないか

リーチしたとき捨て牌を決めた後、待ちがくるまでツモ切りする

*****************************************************
		プレイヤー３手牌
		河0 0 0 0 0 0 

プレイヤー２手牌			プレイヤー４手牌
河0 0 0 0 0 0			河0 0 0 0 0 0 


		河0 0 0 0 0 0
		 
		プレイヤー１手牌
1  2  3  4  5  6  7  8  9  10  11  12  13  14

*****************************************************
	
*/












