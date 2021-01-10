#include<string.h>
#include<stdio.h>
#include<stdlib.h>

struct Dataset{
	char description[100];
	int invoice;
	double unit_price;
	int quantity;
};
struct Dataset trans[6000];

//////////////////////////////////////////////////////////////////////////////////////////////////

float find_trans(){ //No of transactions
	float count=0;
	for(int i=0;i<533;i++){
		if(trans[i].invoice!=trans[i+1].invoice){
			count++;
		}
	}
	return count;
}

struct node{
	char desc[100];
	float freq;
	float sup;
	struct node* next;
};
struct node* head = NULL;
struct node* last = NULL;

void insertData(const char* desc, int freq){ //Insert data in item Node
	struct node* ptr;
	ptr = (struct node*) malloc(sizeof(struct node));
	ptr->next = NULL;
	strcpy(ptr->desc,desc);
	ptr->freq = freq;
	if(head==NULL){
		head=last=ptr;
	}
	else{
		last->next = ptr;
		last = ptr;
	}
	
}

int searchItem(const char*desc){ //Search Data in Item Node
	if (head==NULL)
		return 0;
	struct node* ptr;
	ptr = head;
	int count=0;
	while(ptr!=NULL){
		count++;
		if(strcmp(desc,ptr->desc)==0)
			break;
		else
			ptr=ptr->next;
	}
	return count;
}

float ret_sup(const char*desc){//returns support of the given item
	 if (head==NULL)
		return 0;
	struct node* ptr;
	ptr = head;
	int sup=0;
	while(ptr!=NULL){
		if(strcmp(desc,ptr->desc)==0)
			return ptr->sup;
		else
			ptr=ptr->next;
	}
	return 1;
}

int find_len(){ //Find No of items
	if (head==NULL)
		return 0;
	struct node* ptr;
	ptr = head;
	int count=0;
	while(ptr!=NULL){
		count++;
		ptr=ptr->next;
	}
	return count;
}
void eliminate(){ //eliminate all items with frequencies less than 2
	struct node* ptr;
	struct node* temp;
	ptr=head;
	temp=ptr->next;
	while(temp!=NULL){
		if(temp->freq==1){
			if(temp->next==NULL){
				last=ptr;
			}
			ptr->next=temp->next;
			free(temp);
			temp= ptr->next;
		}
		else{
			temp = temp->next;
			ptr=ptr->next;
		}
	}
	if(head->freq==1){
		ptr=head;
		head=head->next;
		free(ptr);
	}
}
void find_freq(){ //find frequencies of all items
	int i,count,j,len;
	for(i=0;i<533;i++){
		count = searchItem(trans[i].description);
		len = find_len();
		if(count==1 || count==0)
			insertData(trans[i].description,1);
		else if(count==len){
			insertData(trans[i].description,1);
		}
		else{
			struct node* ptr;
			ptr = head;
			for(j=1;j<count;j++)
				ptr = ptr->next;
			ptr->freq = ptr->freq+1;
		}
	}
	eliminate();
}
void find_sup(){ //CALCULATE SUPPORT
	find_freq();
	struct node * temp;
	temp=head;
	float n1 = find_trans();
	while(temp!=NULL){
		temp->sup = temp->freq/n1 ;
		
		temp = temp->next;
	}
	
}


void print(){ //print items
	if (head==NULL)
		return;
	struct node* ptr;
	ptr = head;
	while(ptr!=NULL){
		printf("%f %f %s \n",ptr->freq,ptr->sup,ptr->desc);
		ptr=ptr->next;
	}
}
	
///////////////////////////////////////////////////////////////////////////////////////////////////

struct ItemCombo{
	char desc1[100];
	char desc2[100];
	float freq;
	float conf;
	float lift;
	struct ItemCombo* next;
};

struct ItemCombo* head1 = NULL;
struct ItemCombo* last1 = NULL;

void insertItem(const char* desc1, const char* desc2, int freq){//insert new itemcombo
	struct ItemCombo* ptr;
	ptr = (struct ItemCombo*) malloc(sizeof(struct ItemCombo));
	ptr->next = NULL;
	strcpy(ptr->desc1,desc1);
	strcpy(ptr->desc2,desc2);
	ptr->freq = freq;
	if(head1==NULL){
		head1=last1=ptr;
	}
	else{
		last1->next = ptr;
		last1 = ptr;
	}
	
	
}

int search(const char* desc,int size, struct Dataset d[size]){
	for(int i=0;i<size;i++){
		if(strcmp(d[i].description,desc)==0)
			return 1;
	}
	return 0;
}
	
void combo_freq(){ //find frequency of combo
	struct Dataset d[1000];
	int index=0,freq=0,i,j,len,k;
	len = find_len();
	struct node* ptr;
	ptr=head;
	struct node *temp;
	while(ptr!=NULL){
		temp=ptr->next;
		while(temp!=NULL){
			for(k=0;k<533;k++){
				if(trans[k].invoice==trans[k+1].invoice){
					d[index].invoice=trans[k].invoice;
					strcpy(d[index].description,trans[k].description);
					d[index].unit_price = trans[k].unit_price;
					d[index].quantity = trans[k].quantity;
					index++;
				}
				else{
					d[index].invoice=trans[k].invoice;
					strcpy(d[index].description,trans[k].description);
					d[index].unit_price = trans[k].unit_price;
					d[index].quantity = trans[k].quantity;
					index++;
					if(search(ptr->desc,index+1,d)==1 && search(temp->desc,index+1,d)==1){
						freq++;
						
					}
					index=0;
				}
			}
			
			insertItem(ptr->desc, temp->desc,freq);
			freq=0;
			temp=temp->next;
		}
		ptr=ptr->next;
	}
	
}

void confidence(){//CALCULATE CONFIDENCE
	combo_freq();
	struct ItemCombo* ptr;
	struct node* temp;
	ptr = head1;
	float freq=1;
	while(ptr!=NULL){
		temp=head;
		while(temp!=NULL){
			if(strcmp(ptr->desc1,temp->desc)==0){
				freq = temp->freq;
				break;
			}
			temp=temp->next;
		}	
	
		ptr->conf = ptr->freq/freq;
		ptr = ptr->next;
	}
}
void printItem(){//print itemcombo
	if (head1==NULL)
		return;
	struct ItemCombo* ptr;
	ptr = head1;
	while(ptr!=NULL){
		printf("%f %f %f %s  %s \n",ptr->freq,ptr->conf,ptr->lift,ptr->desc1,ptr->desc2);
		ptr=ptr->next;
	}
}
void eliminate_combo(){//eliminate itemcombo with a confidence<.9
	struct ItemCombo* temp;
	struct ItemCombo* ptr;
	temp = head1;
	ptr=head1->next;
	while(ptr!=NULL){
		if(ptr->conf<.5){
			temp->next=ptr->next;
			if (ptr==last1){
				last1=temp;
			}
			free(ptr);
			ptr=temp->next;
		}
		else{
			temp=temp->next;
			ptr=ptr->next;
		}
	}
	ptr = head1;
	if(head1->conf<.5){
		head1 = head1->next;
		free(ptr);
	}
}
int count_combo(){//count combinations
	struct ItemCombo* ptr;
	int count=0; 
	ptr=head1;
	while(ptr!=NULL){
		count++;
		ptr=ptr->next;
	}
	return count;
}

void apriori_(){//apriori algo calculation of lift
	eliminate_combo();
	struct ItemCombo *ptr;
	ptr = head1;
	float lift,sup;
	while(ptr!=NULL){
		sup = ret_sup(ptr->desc2);
		lift = ptr->conf/sup;
		ptr->lift = lift;
		ptr = ptr->next;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int power(int x,int y){
	int num=1;
	for (int i=1;i<=y;i++)
		num=num*x;
	return num;
}

int retNum(const char* tok){
	int length=strlen(tok),i,num=0,digit,j;
	for(i=0,j=length-1;i<length,j>-1;i++,j--){
		digit=tok[i]-'0';
		num=num+digit*power(10,j);
	}
	return num;
}

void populate(){
	FILE* stream = fopen("market.csv", "r");
	char line[1024];
	const char* tok;
	int num=0,dis=533,i=0,val;
    	while (fgets(line, 1024, stream)){
        	char* tmp = strdup(line);
		val=0;
		if(num!=0){
        		for (tok = strtok(line, ","); tok && *tok; tok=strtok(NULL, ",\n")){
				if(val==1)
					strcpy(trans[i].description,tok);
					
				else if(val==0)
					trans[i].invoice=retNum(tok);
				else if(val==2)
					trans[i].quantity=retNum(tok);
				else if(val==3)
					trans[i].unit_price = retNum(tok);
				val++;
			}
			i++;
		}
		num++;
        	free(tmp);
    	}
	
}




void main(){
populate();
find_sup();
print();
confidence();
printf("%d \n",count_combo());
apriori_();
printf("%d \n",count_combo());
printItem();
}
