#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include "signature.h"
#define maxSize 1024

void isdir(char *dir,Sig *sigs,int index); //isdir 함수 사용을 위해 선언

void main(int argc, char *argv[]) {
	FILE *fp; //읽어올 파일의 포인터
	FILE *sfp; // 시그니처파일 포인터


	char sigline[maxSize] = ""; // 시그니처 파일의 한 라인을 읽어 저장할 문자열
	int sigcount=0; // 시그니처 코드의 개수
	Sig sigs[10]; // 시그니처 코드 와 타입을 저장할 구조체 배열

	sfp = fopen("file_sig_data", "r"); //시그니처 파일 열기
	

	int index =0; //시그니처 구조체 배열의 인덱스 위치

	// 시그니처 코드 저장 코드 --------------------------------
	while (fgets(sigline,sizeof(sigline),sfp) != NULL) {
		if(*(sigline) == '#'){sigcount++; continue;} // 읽어온 문자열 맨 앞에 '#'이 존재 하면 읽지 않는다.
		Sig sig; // 읽어온 문자열을 나눠 저장 할 구조체
		char *temp = strtok(sigline,"|"); // "|"을 기준으로 왼쪽의 문자열을 임시로 저장.
		strcpy(sig.code,temp); // sig 구조체의 code문자열에 저장
		temp = strtok(NULL, "\n"); // "|"뒤의 문자열을 저장
		strcpy(sig.type,temp); // sig 구조체의 type문자열에 저장
		char  *reserve = sig.code; // sig구조체의 code문자열을 임시 문자열에 저장
		temp = strtok(reserve," "); // 임시 문자열을" "기준으로 나눈다.
		strcpy(sig.code,temp); // 그리고 나눈 문자열을 sig구조체의 code문자열에 다시 저장
		temp = strtok(NULL, " ");
		while(temp != NULL) {
			strcat(sig.code,temp);
			temp = strtok(NULL, " ");
		} // 나눌게 존재 하지 않을 때까지 " " 기준으로 나눠서 sig구조체 code문자열에 이어 붙이기
		sigs[index++] = sig; //sigs구조체 배열에 저장후 다음에 저장 될 구조체를 위해 index증가		
		sigcount++; // filesig_length에 프린트 될 변수 증가
	}//--------------------------------------------------------


	printf("filesig_length = %d : ",sigcount);
	for(int i = 0;i< index;i++) {	printf("[%s] ",sigs[i].type);}
	printf("\n");

	if((fp = fopen(argv[1], "rb")) == NULL ) {
		fprintf(stderr,"Usage: ./main (filename | dirname)\n");
		exit(0);
	} //파일 열기에 실패하면 사용법 출력
	else {
		struct stat buf;
		lstat(argv[1],&buf);
		char findpath[1024] = ""; //파일 경로 저장할 변수
		sprintf(findpath,"%s",argv[1]); // 파일 경로 저장

		//파일이라면
		if(S_ISREG(buf.st_mode)) {
			char temp[20]= ""; //16진수로 변환한 2진코드를 저장할 문자열
			char c; // 이진코드를 char크기만큼 저장할 문자 변수
			for (int j=0;j<index;j++) {
				char reserve[2]; //16진수로 변환된 2진코드를 저장할 문자열
				c = fgetc(fp); // char크기 만큼 2진코드 뽑기
				sprintf(reserve,"%X",c); // reserve 문자열에 16진수로 변환해 저장
				strcat(temp,reserve); // temp문자열에 이어붙이기
			} 
			for(int j=0;j<index;j++) {
				if(strstr(temp,sigs[j].code) != NULL) {
					printf("File type of %s is %s.\n",findpath,sigs[j].type);
					break;
				} // sigs구조체배열의 구조체들의 code문자열 중 같은게 존재하면 프린트
			}
			fclose(fp);
		} // 디렉토리라면
		else {	
			isdir(findpath,sigs,index); // 디렉토리 함수 호출
			}	
		}	
	
	fclose(sfp);
}
