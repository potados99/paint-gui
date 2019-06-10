/**
 * metric.h
 * 점이나 크기와 같은 규격들을 정의해놓았습니다.
 */

#ifndef metric_h
#define metric_h

struct point {
	short x;
	short y;
};

struct size {
	short width;
	short height;
};

/**
 * 점을 표현하는 연결리스트에서 쓰일 노드.
 */
struct point_node {
    short x;
    short y;
    struct point_node *next;
};

/**
 * 연결리스트의 시작과 끝을 가지고 있는 '연결리스트' 그 자체.
 * 리스트를 가지고 다닐 때에는 이놈만 씁니당.
 */
struct points {
    struct point_node *head;
    
    /*
     * head만 가지고 다니면 될 것을, 굳이 새 구조체를 정의해서 이렇게 들고다니는 이유:
     *
     * 1. Node만 봐서는 이게 시작인지 끝인지도 몰라 이것을 연결리스트로 보기에는 대표성이 없다.
     * 2. 새 노드를 추가할 때 기존 리스트의 마지막 노드에 연결해야 하는데, 매번 head부터 끝까지 찾는 것은 낭비이므로,
     *  tail을 미리 가지고 있으면서 O(1) 시간 내에 추가 연산이 가능하도록 함.
     */
    struct point_node *tail;
};

void points_add(struct points *points, short x, short y);

void points_free(struct points *points);

#endif /* metric_h */
