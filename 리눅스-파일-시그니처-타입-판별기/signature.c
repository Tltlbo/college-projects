#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>

typedef struct  {
	char code[256];
	char type[256];
} Sig; //함수 내에서 Sig구조체 사용을 위한 선언

void isdir(char *dir,Sig* sigs, int index) {		
	DIR *dp = opendir(dir); // 디렉토리 열기
	if(dp == NULL) return; // 열기 실패 시 리턴

	struct dirent *d;
	while((d = readdir(dp)) != NULL) {
		if(strcmp(d->d_name,".") == 0 || strcmp(d->d_name,"..") == 0) continue; // 현재 디렉토리나 부모 디렉토리가 입력 될시 넘어간다.
		struct stat buf; // 파일 정보를 저장할 buf 변수
		char path[1024]; // 경로를 저장해줄 변수 path
		sprintf(path,"%s/%s",dir,d->d_name); // path에 현재 경로 저장

		if(lstat(path,&buf) < 0) {} // 파일정보를 못가져왔을 경우 
		else {
			if(S_ISDIR(buf.st_mode)) isdir(path,sigs,index); // 디렉토리인 경우 재귀호출
			else {
				FILE *fp = NULL;
				char c;
				char temp[20] = ""; //파일의 시그니처 코드를 저장할 문자열
				if ((fp = fopen(path, "rb")) == NULL) continue; 
				//파일을 열지 못하는 경우 넘어가기
				else {
					for(int i=0;i<index;i++) {
						char reserve[2];
						c = fgetc(fp); // char크기만큼 이진코드 뽑아오기
						sprintf(reserve,"%X",c);
					       	//뽑아온 이진코드를 16진수로 변환해 임시 문자열에 저장
						strcat(temp,reserve); // temp문자열에 이어 붙이기
					} // 안전하게 헤더코드를 뽑기 위해 index만큼 반복
					for(int j=0;j<index;j++) {
						if(strstr(temp,sigs[j].code) != NULL) {
							printf("File type of %s is %s. \n",path,sigs[j].type);
							break;
						} //뽑아온 문자열에 시그니처데이터 파일의 문자열과 비교해 같은게 존재하면 출력
					}

				}
				fclose(fp);
			}
		}
	}
	closedir(dp);
}
