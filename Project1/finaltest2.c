#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. 메모리 상에서 사용할 연결 리스트 노드 구조체
typedef struct Student {
    char name[20];
    int kor;
    int eng;
    int math;
    int total;
    float avg;
    int rank;
    struct Student* next; // 연결 리스트용 포인터
} Student;

// 2. 파일 저장/읽기용 구조체 (포인터 제외, 파일 형식과 1:1 매칭)
typedef struct {
    char name[20];
    int kor;
    int eng;
    int math;
    int total;
    float avg;
    int rank; // 파일 끝에 있는 4바이트(CC CC CC CC)를 받아낼 공간
} FileData;

// 전역 변수: 리스트의 머리(Head)
Student* head = NULL;

// 함수 선언
void printMenu();
void loadFile();
void addStudent();
void saveFile();
void printGrades();
void freeMemory();
void calculateStats(Student* s);

int main() {
    int choice;

    // 프로그램 시작 시 파일이 있으면 자동으로 읽어오기 (선택 사항)
    loadFile();

    while (1) {
        printMenu();
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            loadFile();
            break;
        case 2:
            addStudent();
            break;
        case 3:
            saveFile();
            break;
        case 4:
            printGrades();
            break;
        case 5:
            printf("프로그램을 종료합니다.\n");
            saveFile(); // 종료 전 자동 저장
            freeMemory();
            return 0;
        default:
            printf("잘못된 선택입니다. 다시 입력해주세요.\n");
        }
    }
    return 0;
}

void printMenu() {
    printf("\n[Menu]\n");
    printf("1. students.dat 파일에서 데이터 읽기 (새로고침)\n");
    printf("2. 추가 학생 정보 입력\n");
    printf("3. students.dat 파일 저장\n");
    printf("4. 성적 확인 (평균/석차 계산)\n");
    printf("5. 종료\n");
    printf("------------------------\n");
    printf("선택(1~5): ");
}

// 총점, 평균 계산 도우미 함수
void calculateStats(Student* s) {
    s->total = s->kor + s->eng + s->math;
    s->avg = s->total / 3.0f;
    s->rank = 1; // 기본값
}

// 1. 파일 읽기 (업로드해주신 students.dat 형식 맞춤)
void loadFile() {
    FILE* fp = fopen("students.dat", "rb");
    if (fp == NULL) {
        printf("\n[알림] 기존 데이터 파일(students.dat)이 없습니다. 새로 시작합니다.\n");
        return;
    }

    // 기존 메모리 초기화
    freeMemory();

    FileData temp;
    int count = 0;

    while (fread(&temp, sizeof(FileData), 1, fp)) {
        Student* newNode = (Student*)malloc(sizeof(Student));

        // 파일 데이터 -> 리스트 노드로 복사
        strcpy(newNode->name, temp.name);
        newNode->kor = temp.kor;
        newNode->eng = temp.eng;
        newNode->math = temp.math;

        // 파일에 저장된 총점/평균을 그대로 쓸 수도 있지만, 신뢰성을 위해 재계산
        calculateStats(newNode);

        newNode->next = NULL;

        // 리스트 연결
        if (head == NULL) {
            head = newNode;
        }
        else {
            Student* cur = head;
            while (cur->next != NULL) {
                cur = cur->next;
            }
            cur->next = newNode;
        }
        count++;
    }

    fclose(fp);
    printf("\n[성공] %d명의 학생 데이터를 불러왔습니다.\n", count);
}

// 2. 학생 추가
void addStudent() {
    Student* newNode = (Student*)malloc(sizeof(Student));

    printf("\n[학생 정보 입력]\n");
    printf("이름: ");
    scanf("%s", newNode->name);
    printf("국어 점수: ");
    scanf("%d", &newNode->kor);
    printf("영어 점수: ");
    scanf("%d", &newNode->eng);
    printf("수학 점수: ");
    scanf("%d", &newNode->math);

    calculateStats(newNode); // 총점/평균 즉시 계산
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    }
    else {
        Student* cur = head;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = newNode;
    }

    printf("학생 정보가 추가되었습니다.\n");
}

// 3. 파일 저장 (구조체 전체 저장)
void saveFile() {
    FILE* fp = fopen("students.dat", "wb");
    if (fp == NULL) {
        printf("파일 저장 중 오류가 발생했습니다.\n");
        return;
    }

    // 등수 재계산 후 저장 (최신 상태 반영)
    // 1. 등수 계산 로직 수행
    Student* i = head;
    while (i != NULL) {
        i->rank = 1;
        Student* j = head;
        while (j != NULL) {
            if (j->total > i->total) {
                i->rank++;
            }
            j = j->next;
        }
        i = i->next;
    }

    // 2. 파일에 쓰기
    Student* cur = head;
    FileData temp; // 파일용 구조체

    while (cur != NULL) {
        strcpy(temp.name, cur->name);
        temp.kor = cur->kor;
        temp.eng = cur->eng;
        temp.math = cur->math;
        temp.total = cur->total;
        temp.avg = cur->avg;
        temp.rank = cur->rank; // 계산된 등수 저장

        fwrite(&temp, sizeof(FileData), 1, fp);
        cur = cur->next;
    }

    fclose(fp);
    printf("현재 데이터를 students.dat 파일에 저장했습니다.\n");
}

// 4. 성적 확인
void printGrades() {
    if (head == NULL) {
        printf("출력할 데이터가 없습니다.\n");
        return;
    }

    // 등수 계산 (실시간)
    Student* i = head;
    while (i != NULL) {
        i->rank = 1;
        Student* j = head;
        while (j != NULL) {
            if (j->total > i->total) {
                i->rank++;
            }
            j = j->next;
        }
        i = i->next;
    }

    printf("\n--------------------------------------------------------------\n");
    printf(" 이름    국어  영어  수학   총점    평균   등수\n");
    printf("--------------------------------------------------------------\n");

    Student* cur = head;
    while (cur != NULL) {
        printf("%-8s %3d   %3d   %3d    %3d   %5.1f   %d등\n",
            cur->name, cur->kor, cur->eng, cur->math,
            cur->total, cur->avg, cur->rank);
        cur = cur->next;
    }
    printf("--------------------------------------------------------------\n");
}

void freeMemory() {
    Student* cur = head;
    Student* nextNode;

    while (cur != NULL) {
        nextNode = cur->next;
        free(cur);
        cur = nextNode;
    }
    head = NULL;
}