#ifndef __GAME_H__
#define __GAME_H__

#include<vector>
#include<algorithm>
#include<assert.h>
#include<string.h>
#include<stdio.h>
using std::vector;
using std::sort;
using std::swap;

/*

	tiles:1-9m,1-9p,1-9s,1-7z=1-34;0m0p0s=35-37

*/

char tiletype(int x){
	if(x<=34)
		return "mpsz"[(x-1)/9];
	else
		return "mps"[x-35];
}
int tileord(int x){//0=5
	if(x<=34)
		return (x-1)%9+1;
	else
		return 5;
}
int make_tile(int ord,char tp){
	int t=-1;
	if(tp=='m')
		t=0;
	if(tp=='p')
		t=1;
	if(tp=='s')
		t=2;
	if(tp=='z')
		t=3;
	assert(t!=-1);
	if(ord==0){
		assert(tp!='z');
		return 35+t;
	}else
		return t*9+ord;
}
bool tileequal(int a,int b){//0=5
	return tiletype(a)==tiletype(b)&&tileord(a)==tileord(b);
}

struct meld{
	vector<int> id;
	int from;
	meld(){
		id.clear();
		from=0;
	}
	meld(vector<int> _id,int _from):id(_id),from(_from){}
};
meld make_chi(int a,int b,int c){
	if(a>b)
		swap(a,b);
	int r[3]={c,a,b};
	return meld(vector<int>(r,r+3),3);
}
meld make_pon(int a,int b,int c,int from){
	int r[3]={a,b,c};
	swap(r[2],r[3-from]);
	return meld(vector<int>(r,r+3),from);
}
meld make_kan(int a,int b,int c,int from){
	int r[4]={a,b,b,c};
	if(from>1)
		swap(r[3],r[from-1]);
	return meld(vector<int>(r,r+4),from);
}

struct playerinfo;

struct meld_option{
	int type;//1:chi,2:pon,3:kan
	int a,b,c,d;
	/*
		chi:tiles=a,b,c;from=3;kuigae=c,d
		pon:tiles=a,b,c;from=d;kuigae=c
		kan:tiles=a,b,b,c;from=d
	*/
	meld_option(){
		type=0;
		a=b=c=d=0;
	}
	meld_option(int _type,int _a,int _b,int _c,int _d):
		type(_type),a(_a),b(_b),c(_c),d(_d){}
	void execute(playerinfo*p);

	void print(){
		if(type==1)
			printf("%d %d chi %d\nkuigae=%d,%d\n",a,b,c,c,d);
		if(type==2)
			printf("%d %d pon %d\nkuigae=%d\n",a,b,c,c);
		if(type==3)
			printf("%d %d %d kan %d\n",a,b,b,c);
	}
};
bool operator<(const meld_option& a,const meld_option& b){
	return a.type<b.type;
}

struct playerinfo{
	int hand[38];
	vector<meld> melds;
	vector<int> discard;
	bool riichi;
	playerinfo(){
		memset(hand,0,sizeof(hand));
		melds.clear();
		discard.clear();
		riichi=0;
	}
	void execute_chi(int a,int b,int c){
		assert(!riichi);
		hand[a]--;
		hand[b]--;
		melds.push_back(make_chi(a,b,c));
	}
	bool can_chi(int a,int b,int c,int&d){
		if(riichi)
			return 0;
		if(hand[a]==0||hand[b]==0)
			return 0;
		char tp=tiletype(a);
		if(tiletype(b)!=tp||tiletype(c)!=tp)
			return 0;
		assert(tp!='z');
		int t[3];
		t[0]=tileord(a);
		t[1]=tileord(b);
		t[2]=tileord(c);
		sort(t,t+3);
		d=0;
		if(tileord(c)==t[2]&&t[0]>1)
			d=make_tile(t[0]-1,tp);
		if(tileord(c)==t[0]&&t[2]<9)
			d=make_tile(t[2]+1,tp);
		int h[38];
		memcpy(h,hand,sizeof(hand));
		h[a]--;
		h[b]--;
		for(int i=0;i<3;i++){
			h[i*9+5]+=h[35+i];
			h[35+i]=0;
		}
		for(int i=1;i<=34;i++){
			if(i!=d&&i!=c&&h[i]>0)
				return 1;
		}
		return 0;
	}
	void execute_pon(int a,int b,int c,int from){
		assert(!riichi);
		hand[a]--;
		hand[b]--;
		melds.push_back(make_pon(a,b,c,from));
	}
	void execute_kan(int a,int b,int c,int from){
		hand[a]--;
		hand[b]-=2;
		melds.push_back(make_kan(a,b,c,from));
	}
	vector<meld_option> chi_list(int x){
		int o=tileord(x);
		char tp=tiletype(x);
		assert(tp!='z');
		int p,d,t0;
		vector<meld_option> r;
		t0=make_tile(0,tp);
		if(o>=3){
			p=make_tile(o-2,tp);
			if(can_chi(p,p+1,x,d))
				r.push_back(meld_option(1,p,p+1,x,d));
			if(o-2==5&&can_chi(t0,p+1,x,d))
				r.push_back(meld_option(1,t0,p+1,x,d));
			if(o-1==5&&can_chi(p,t0,x,d))
				r.push_back(meld_option(1,p,t0,x,d));
		}
		if(o>=2&&o<=8){
			p=make_tile(o-1,tp);
			if(can_chi(p,p+2,x,d))
				r.push_back(meld_option(1,p,p+2,x,d));
			if(o-1==5&&can_chi(t0,p+2,x,d))
				r.push_back(meld_option(1,t0,p+2,x,d));
			if(o+1==5&&can_chi(p,t0,x,d))
				r.push_back(meld_option(1,p,t0,x,d));
		}
		if(o<=7){
			p=make_tile(o+1,tp);
			if(can_chi(p,p+1,x,d))
				r.push_back(meld_option(1,p,p+1,x,d));
			if(o+1==5&&can_chi(t0,p+1,x,d))
				r.push_back(meld_option(1,t0,p+1,x,d));
			if(o+2==5&&can_chi(p,t0,x,d))
				r.push_back(meld_option(1,p,t0,x,d));
		}
		return r;
	}
	vector<meld_option> pon_list(int x,int from){
		char tp=tiletype(x);
		int o=tileord(x);
		vector<meld_option> r;
		if(hand[x]>=2)
			r.push_back(meld_option(2,x,x,x,from));
		if(tp!='z'&&o==5){
			int t0=make_tile(0,tp),t5=make_tile(5,tp);
			if(x==t0&&hand[t5]>=2)
				r.push_back(meld_option(2,t5,t5,t0,from));
			if(x==t5&&hand[t5]>0&&hand[t0]>0)
				r.push_back(meld_option(2,t0,t5,t5,from));
		}
		return r;
	}
	vector<meld_option> kan_list(int x,int from){
		char tp=tiletype(x);
		int o=tileord(x);
		vector<meld_option> r;
		if(hand[x]>=3)
			r.push_back(meld_option(3,x,x,x,from));
		if(tp!='z'&&o==5){
			int t0=make_tile(0,tp),t5=make_tile(5,tp);
			if(x==t0&&hand[t5]>=3)
				r.push_back(meld_option(3,t5,t5,t0,from));
			if(x==t5&&hand[t5]+hand[t0]>=3)
				r.push_back(meld_option(3,t0,t5,t5,from));
		}
		return r;
	}
};

void meld_option::execute(playerinfo*p){
	if(type==1)
		p->execute_chi(a,b,c);
	if(type==2)
		p->execute_pon(a,b,c,d);
	if(type==3)
		p->execute_kan(a,b,c,d);
}

#endif