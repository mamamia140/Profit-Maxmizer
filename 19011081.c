#include<stdio.h>
#include<stdlib.h>

typedef struct ad{ // reklamlarýn bilgilerini tutmak için AD isminde bir struct oluþturdum.
    int startTime;
    int duration;
    int value;
    int finishTime;
    int place;
}AD;

void merge(AD* array,int low,int mid,int high){ // merge sort algoritmasýnýn merge fonksiyonu
    int n1 = mid - low + 1;
    int n2 = high - mid;
    int i,j,k;
    AD left[n1], right[n2];

    for (i = 0; i < n1; i++){
        left[i] = array[low + i];
    }
    for (j = 0; j < n2; j++){
        right[j] = array[mid + 1 + j];
    }
    i = 0;
    j = 0;
    k = low;

    while (i < n1 && j < n2) {
        if (left[i].finishTime <= right[j].finishTime) {
            array[k] = left[i];
            i++;
        } 
        else {
            array[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        array[k] = left[i];
        i++;
        k++;
    }

    while (j < n2) {
        array[k] = right[j];
        j++;
        k++;
    }
    
}

void sortByFinishTime(AD* array,int low,int high){ // merge sort kullanarak reklamlarý bitiþ süresine göre sýralayan fonksiyon
    if(low < high){
        int mid = (low + high)/2;
        sortByFinishTime(array,low,mid);
        sortByFinishTime(array,mid+1,high);
        merge(array,low,mid,high);
    }
}

int** solution(AD* array,int size){ // dinamik programlama ile maksimum kazancý bulan algoritmanýn fonksiyonu
    int highestFinishTime = array[size-1].finishTime; // matris kaç sütunlu yapacaðýmý bilmek için en yüksek bitiþ zamanýný bir deðiþkene aldým.
    int i,j;
    int** profitTable = (int**)malloc(size * sizeof(int*)); // profit matrisi oluþturuyorum
    for (i = 0; i < size; i++){
        profitTable[i] = (int*)malloc((highestFinishTime+1) * sizeof(int));
    }
    for(j=0;j<array[0].finishTime;j++){ // ilk reklamýn bitiþ süresine kadar kýsýmlarý sýfýrlýyorum.
        profitTable[0][j]=0;
    }
    for(j=array[0].finishTime;j<highestFinishTime;j++){ // ilk reklamýn bitiþ süresine ve ondan sonraki yerlere o reklamýn deðerini yazýyorum.
        profitTable[0][j] = array[0].value;
    }
    
    for(i=1;i<size;i++){ 
        for(j=0;j<array[i].finishTime;j++){ //Array[i].finishTime'a kadar üstten kopyalýyorum.
           profitTable[i][j] = profitTable[i-1][j];
        }
        
        if(array[i].value + profitTable[i-1][j-array[i].duration] > profitTable[i-1][j]){ //reklam seçilecek mi? selçilmeyecek mi? kararýný veriyorum.
            profitTable[i][j] = profitTable[i-1][j-array[i].duration] + array[i].value; // seçildiði için kendisinden önce ve çakýþmayan reklamlarýn gelirlerine eklenip gerekli yer dolduruldu.
        }
        else{ 
            profitTable[i][j] = profitTable[i][j-1]; // reklam seçilmedi. bir önceki deðer aynen yazýldý.
        }
        for(j=array[i].finishTime + 1;j<=highestFinishTime;j++){ //reklamýn bitiþ süresinden sonraki süreler bir önceki deðerle ayný þekilde yazýldý.
            profitTable[i][j] = profitTable[i][j-1];
        }
    }

    return profitTable;
}

int* findChosenAds(int** profitTable,AD* array,int maxRevenue,int size){ // seçilen reklamlarý bulmaya yarayan fonksiyon.
	int i,j,k=0;
	int *chosenAds = (int*)calloc(size,sizeof(int)); // seçilen reklamlarýn numaralarýný tutmak için bir dizi oluþturuyorum.
    i = size-1;
    j = array[size-1].finishTime;
    while(maxRevenue != 0 && i != 0){ 
        if(profitTable[i][j] != profitTable[i-1][j]){ // ilgili reklam seçilmiþtir. j indisine o reklamýn baþlangýç süresi atanýr.
            chosenAds[k] = array[i].place;
            k++;
            j = array[i].startTime;
            maxRevenue = maxRevenue - array[i].value;
        }
        i--; // reklam seçilse de seçilmese de i bir azaltýlýr.
    }

    if(maxRevenue != 0){
        chosenAds[k] = array[0].place;
    }
	return chosenAds;
}

int main(){
    AD *array = (AD*)malloc(100 * sizeof(AD)); // reklam bilgilerini saklamak için AD türünde bir dizi oluþturdum.
    FILE* file = fopen("Sample.txt","r");
    int size=0,i,maxRevenue;
    int** profitTable; //ilerde solution fonksiyonu yardýmýyla bulacaðým gelir tablosunu tutmak için int** türünden bir pointer oluþturdum.
    int* chosenAds; //ilerde hesaplayýp bulacaðým seçilmiþ reklamlarýn numarasýný tutmak için bir int* türünden bir pointer oluþturdum.
    
    while(fscanf(file,"%d%d%d", &(array[size].startTime),&(array[size].duration),&(array[size].value)) != EOF){ // teker teker tüm reklam bilgilerini okuyup array dizisine yazdým.
        array[size].finishTime = array[size].startTime + array[size].duration;
        array[size].place = ++size;
    } 

    sortByFinishTime(array,0,size-1); // reklamlarý bitiþ süresine göre artan þekilde sýraladým.

    profitTable = solution(array,size); //dinamik programlama yöntemiyle en yüksek kazancý bulan algoritmaya gerekli parametreleri yolladým. Ýþlemler sonucu dönen tabloyu ads adýndaki matrisde sakladým.

    maxRevenue = profitTable[size-1][array[size-1].finishTime]; //ads tablosunun en son satýr ve sütunundan maksimum geliri buldum.

    printf("max_value = %d\n",maxRevenue); // maksimum geliri yazdýrdým.

    chosenAds = findChosenAds(profitTable,array,maxRevenue,size); //ads tablosundan seçilen reklamlarý bulmak için bir fonksiyon yazdým.
    
    i=0;
    printf("chosen ads are "); //sýrayla seçilen tüm reklamlarý yazdýrdým.
    while(chosenAds[i] != 0){
        printf("ad number %d,",chosenAds[i]);
        i++;
    }

    return 0;
}
