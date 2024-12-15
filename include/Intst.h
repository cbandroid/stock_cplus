#ifndef TECHINDEX_INTST_H
#define TECHINDEX_INTST_H

template <class TY> class Intem
{
public:
	Intem(TY v=0){val=v;next=0;prev=0;}
	Intem<TY>* next;
	Intem<TY>* prev;
	TY val;
};

template <class TH> class Intst 
{
public:
	Intst(TH val) { list = new Intem<TH>(val);tail=0;};
	Intst() {list = 0;tail=0;};
//===========压入头部=============
	TH push_b(TH val)
	{
		Intem<TH>* pt;
		pt=new Intem<TH>(val);
		pt->next = list;
		if(list!=0) 
			list->prev = pt;
		else
			tail = pt;
		list = pt;
		return val;
	}
//==========压入尾部================
/*	TH push_t(TH val)
	{
		Intem<TH> *pi,*pt,*prv;
		for(prv=pi=list;pi;prv=pi,pi=pi->next)  ;
    
		pt=new Intem<TH>(val);	
		if(list==0)
			list=pt;
		else
		{
			pt->prev=prv;
			prv->next=pt;
		}
		return val;
	}
*/
//========弹出===============
	TH pop_it()
	{
	Intem<TH>* tmp;
	TH val;
	if(list==0)
		return 0;
	tmp=list;
	list=list->next;
	if(list!=0)
		list->prev=0;
	else
		tail=0;
	val = tmp->val;
	delete tmp;
	return val;
	}
//========压出===============
	TH rec_it()
	{
		Intem<TH>* tmp;
		TH val;
		if(tail==0)
			return 0;
		tmp=tail;
		tail=tail->prev;
		if(tail!=0) 
			tail->next=0;
		else
			list=0;
		val = tmp->val;
		delete tmp;
		return val;
	}
private:
	Intem<TH>* list;
	Intem<TH>* tail;
};

#endif