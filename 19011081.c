#include<stdio.h>
#include<stdlib.h>

typedef struct ad{ // reklamlar�n bilgilerini tutmak i�in AD isminde bir struct olu�turdum.
    int startTime;
    int duration;
    int value;
    int finishTime;
    int place;
}AD;

void merge(AD* array,int low,int mid,int high){ // merge sort algoritmas�n�n merge fonksiyonu
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

void sortByFinishTime(AD* array,int low,int high){ // merge sort kullanarak reklamlar� biti� s�resine g�re s�ralayan fonksiyon
    if(low < high){
        int mid = (low + high)/2;
        sortByFinishTime(array,low,mid);
        sortByFinishTime(array,mid+1,high);
        merge(array,low,mid,high);
    }
}

int** solution(AD* array,int size){ // dinamik programlama ile maksimum kazanc� bulan algoritman�n fonksiyonu
    int highestFinishTime = array[size-1].finishTime; // matris ka� s�tunlu yapaca��m� bilmek i�in en y�ksek biti� zaman�n� bir de�i�kene ald�m.
    int i,j;
    int** profitTable = (int**)malloc(size * sizeof(int*)); // profit matrisi olu�turuyorum
    for (i = 0; i < size; i++){
        profitTable[i] = (int*)malloc((highestFinishTime+1) * sizeof(int));
    }
    for(j=0;j<array[0].finishTime;j++){ // ilk reklam�n biti� s�resine kadar k�s�mlar� s�f�rl�yorum.
        profitTable[0][j]=0;
    }
    for(j=array[0].finishTime;j<highestFinishTime;j++){ // ilk reklam�n biti� s�resine ve ondan sonraki yerlere o reklam�n de�erini yaz�yorum.
        profitTable[0][j] = array[0].value;
    }
    
    for(i=1;i<size;i++){ 
        for(j=0;j<array[i].finishTime;j++){ //Array[i].finishTime'a kadar �stten kopyal�yorum.
           profitTable[i][j] = profitTable[i-1][j];
        }
        
        if(array[i].value + profitTable[i-1][j-array[i].duration] > profitTable[i-1][j]){ //reklam se�ilecek mi? sel�ilmeyecek mi? karar�n� veriyorum.
            profitTable[i][j] = profitTable[i-1][j-array[i].duration] + array[i].value; // se�ildi�i i�in kendisinden �nce ve �ak��mayan reklamlar�n gelirlerine eklenip gerekli yer dolduruldu.
        }
        else{ 
            profitTable[i][j] = profitTable[i][j-1]; // reklam se�ilmedi. bir �nceki de�er aynen yaz�ld�.
        }
        for(j=array[i].finishTime + 1;j<=highestFinishTime;j++){ //reklam�n biti� s�resinden sonraki s�reler bir �nceki de�erle ayn� �ekilde yaz�ld�.
            profitTable[i][j] = profitTable[i][j-1];
        }
    }

    return profitTable;
}

int* findChosenAds(int** profitTable,AD* array,int maxRevenue,int size){ // se�ilen reklamlar� bulmaya yarayan fonksiyon.
	int i,j,k=0;
	int *chosenAds = (int*)calloc(size,sizeof(int)); // se�ilen reklamlar�n numaralar�n� tutmak i�in bir dizi olu�turuyorum.
    i = size-1;
    j = array[size-1].finishTime;
    while(maxRevenue != 0 && i != 0){ 
        if(profitTable[i][j] != profitTable[i-1][j]){ // ilgili reklam se�ilmi�tir. j indisine o reklam�n ba�lang�� s�resi atan�r.
            chosenAds[k] = array[i].place;
            k++;
            j = array[i].startTime;
            maxRevenue = maxRevenue - array[i].value;
        }
        i--; // reklam se�ilse de se�ilmese de i bir azalt�l�r.
    }

    if(maxRevenue != 0){
        chosenAds[k] = array[0].place;
    }
	return chosenAds;
}

int main(){
    AD *array = (AD*)malloc(100 * sizeof(AD)); // reklam bilgilerini saklamak i�in AD t�r�nde bir dizi olu�turdum.
    FILE* file = fopen("Sample.txt","r");
    int size=0,i,maxRevenue;
    int** profitTable; //ilerde solution fonksiyonu yard�m�yla bulaca��m gelir tablosunu tutmak i�in int** t�r�nden bir pointer olu�turdum.
    int* chosenAds; //ilerde hesaplay�p bulaca��m se�ilmi� reklamlar�n numaras�n� tutmak i�in bir int* t�r�nden bir pointer olu�turdum.
    
    while(fscanf(file,"%d%d%d", &(array[size].startTime),&(array[size].duration),&(array[size].value)) != EOF){ // teker teker t�m reklam bilgilerini okuyup array dizisine yazd�m.
        array[size].finishTime = array[size].startTime + array[size].duration;
        array[size].place = ++size;
    } 

    sortByFinishTime(array,0,size-1); // reklamlar� biti� s�resine g�re artan �ekilde s�ralad�m.

    profitTable = solution(array,size); //dinamik programlama y�ntemiyle en y�ksek kazanc� bulan algoritmaya gerekli parametreleri yollad�m. ��lemler sonucu d�nen tabloyu ads ad�ndaki matrisde saklad�m.

    maxRevenue = profitTable[size-1][array[size-1].finishTime]; //ads tablosunun en son sat�r ve s�tunundan maksimum geliri buldum.

    printf("max_value = %d\n",maxRevenue); // maksimum geliri yazd�rd�m.

    chosenAds = findChosenAds(profitTable,array,maxRevenue,size); //ads tablosundan se�ilen reklamlar� bulmak i�in bir fonksiyon yazd�m.
    
    i=0;
    printf("chosen ads are "); //s�rayla se�ilen t�m reklamlar� yazd�rd�m.
    while(chosenAds[i] != 0){
        printf("ad number %d,",chosenAds[i]);
        i++;
    }

    return 0;
}
