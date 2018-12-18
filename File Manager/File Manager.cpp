#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fd;
FILE *fk;


typedef struct{
    int id; //Chiave Primaria
    char fName[15]; //Nome
    char lName[15]; //Cognome
}record;

record buffer[100];
record reset={-1, "", ""};
char query[15];
char choice;
int maxIndex;

int getNumRecords(){    //Ritorna il numero di record
    fd=fopen("Data.txt", "r+b");
    long curPos, fileSize;
    curPos = ftell(fd);
    fseek(fd, 0, SEEK_END);
    fileSize=ftell(fd);
    fseek(fd, curPos, 0);
    fclose(fd);
    return (fileSize/sizeof(record)); //Divide la dimensione del file per la dimensione del record
}

void openDataFile(){    //Apre file Dati
    fd = fopen("Data.txt", "a+b");
}

void openKeyFile(){     //Apre file Chiavi
    fk = fopen("Key.txt", "w+b");
}

void closeDataFile(){   //Chiude file Dati
    fclose(fd);
}

void closeKeyFile(){    //Chiude file Chiavi
    fclose(fk);
}

void writeDataFile(){   //Aggiunge record al file Dati
    openDataFile();
    printf("Insert \"END\" to save file and exit \n");
    for(int i=maxIndex; i<=100; i++){
        buffer[i].id=i;
        printf("\nEnter First Name: ");
        scanf("%s", buffer[i].fName); //L
        if(strcmp(buffer[i].fName, "END") == 0){  //Controlla se l'utente inserisce il comando per terminare la scrittura
            printf("\n");
            printf("Stopped Writing Data \n\n");
            break;
        }
        printf("\nEnter Last Name: ");
        scanf("%s", buffer[i].lName); //Legge la stringa inserita
        if(strcmp(buffer[i].lName, "END") == 0){  //Controlla se l'utente inserisce il comando per terminare la scrittura
            printf("\n");
            printf("Stopped Writing Data \n\n");
            break;
        }
        fseek(fd, 0, SEEK_END);
        fwrite(&buffer[i], sizeof(record), 1, fd);    //Scrive sul file il record inserito
    }
    closeDataFile();
}

void readDataFile(){    //Legge il file Dati ed inserisce il suo contenuto nell'array buffer
    openDataFile();
    fread(&buffer, sizeof(record), maxIndex, fd);
    for(int i=0; i<maxIndex; i++){
	    printf ("ID: %d => %s %s\n\n", buffer[i].id, buffer[i].fName, buffer[i].lName);
    }
    printf("\n");
    closeDataFile();
}

void writeKeyFile(){    //Scrive il file Chiavi ordinando il file Dati con la tecnica dell'insertion sort
    record V[100]={0};
    maxIndex=getNumRecords();
    char n[15];
    int j;
    openDataFile();
    fread(&buffer, sizeof(record), maxIndex, fd);
    closeDataFile();

    for(int i=0; i<maxIndex; i++){
        for(int k=0; k<15; k++)
            n[k]=buffer[i].fName[k];
        j=i;
        while(j>0 && strcmp(n, V[j-1].fName)<0){
            V[j]=V[j-1];
            j--;
        }
        V[j]=buffer[i];
    }
    openKeyFile();
    for(int x=0; x<maxIndex; x++){
        fwrite(&V[x], sizeof(record), 1, fk);
    }
    closeKeyFile();
}

void readKeyFile(){ //Legge il file Chiavi ed inserisce il suo contenuto nell'array buffer
    fk = fopen("Key.txt", "a+b");
    fread(&buffer, sizeof(record), maxIndex, fk);
    for(int i=0; i<maxIndex; i++){
	    printf ("%s %s  Pointer: %d \n\n", buffer[i].fName, buffer[i].lName, buffer[i].id );
    }
    printf("\n");
    closeKeyFile();
}

void binarySearch(char query[15]){  //Cerca il record nel file Chiavi con la tecnica della ricerca binaria e stampa i risultati
    int nMin, nMax, nMid;
    fk = fopen("Key.txt", "a+b");
    fread(&buffer, sizeof(record), maxIndex, fk);
    closeKeyFile();
    nMin = 0;
    nMax = maxIndex - 1;
    nMid = (nMin+nMax)/2;

    while (nMin <= nMax) {
        if (strcmp(buffer[nMid].fName, query)<0)
        nMin = nMid + 1;
        else if(strcmp(buffer[nMid].fName, query)==0){
            for(int i=nMid-1; strcmp(buffer[i].fName, query)==0; i--){
                printf("\"%s %s\" found at position %d \n\n", query, buffer[i].lName, buffer[i].id);
            }
            printf("\"%s %s\" found at position %d \n\n", query, buffer[nMid].lName, buffer[nMid].id);
            for(int j=nMid+1; strcmp(buffer[j].fName, query)==0; j++){
                printf("\"%s %s\" found at position %d \n\n", query, buffer[j].lName, buffer[j].id);
            }
            break;
        }
        else
            nMax = nMid - 1;

        nMid = (nMin + nMax)/2;
    }
    if (nMin > nMax)
        printf("\"%s\" can't be found\n", query);
}

void deleteRecord(int idDelete){    //Elimina un record sovrascrivendolo con il record di Reset (record vuoto)
    openDataFile();
    fread(&buffer, sizeof(record), maxIndex, fd);
    closeDataFile();
    fopen("Data.txt", "wb");
    buffer[idDelete]=reset;
    for(int i=idDelete; i<maxIndex; i++){
        buffer[i]=buffer[i+1];
        buffer[i].id-=1;
        buffer[maxIndex]=reset;
    }
    for(int i=0; i<maxIndex; i++){
        if(buffer[i].id > -1)
            fwrite(&buffer[i], sizeof(record), 1, fd);
    }
    closeDataFile();
    writeKeyFile();
    printf("\nDeleted Record\n");
}

int main()
{

    bool exitFlag=false;
    int idDelete;
    char delFlag;


    do{ //Stampa la lista comandi del Menu
        printf("Menu: \n\n");
        printf("    1: Add Record\n\n");
        printf("    2: Search Record\n\n");
        printf("    3: Delete Record\n\n");
        printf("    4: Delete All Records\n\n");
        printf("    5: Print All Records\n\n");
        printf("    6: Print All Keys\n\n");
        printf("    7: Print Total Number of Records\n\n");
        printf("    8: Exit\n\n");

        maxIndex=getNumRecords();
        choice=getchar();
        printf("\n");

        switch(choice){ //Menu

        //Add Record
        case '1' :
        writeDataFile();
        writeKeyFile();
        break;

        //Search Record
        case '2':
        printf("Insert Record to Search:  ");
        scanf("%s", query);
        printf("\n\n");
        binarySearch(query);
        break;

        //Delete Record
        case '3':
        printf("Insert ID of Record to Delete:  ");
        maxIndex=getNumRecords();
        scanf("%d", &idDelete);
        if(idDelete >= maxIndex)
            printf("\n\nERROR: Index not Found\n\n");
        else{
            deleteRecord(idDelete);
            printf("\n\n");
        }
        break;

        //Delete All Records
        case '4':
        printf("\n\nThis action is Irreversible\nAre you Sure? [Y/N]\n\n");
        delFlag=getchar();
        delFlag=getchar();
        if(delFlag == 'Y' || delFlag == 'y'){
            fd = fopen("Data.txt", "w+b");
            closeDataFile();
            writeKeyFile();
            printf("\nDeleted All Records\n\n");
        }
        else{
            printf("\nDeletion of Records has been cancelled\n\n");
        }
        break;

        //Print All Records
        case '5':
        readDataFile();
        break;

        //Print All Keys
        case '6':
        readKeyFile();
        break;

        //Print Number of Records
        case '7':
        printf("Total Records: %d \n\n", getNumRecords());
        break;

        //Exit
        case '8':
        exitFlag=true;
        break;
        }

        choice=getchar();   //Pulisce il buffer della tastiera

    }while(!exitFlag);  //Ripete il menu fino a quando l'utente non inserisce il comando per uscire

    return 0;
}

