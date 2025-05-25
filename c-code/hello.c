#include <stdio.h>
#include <stdlib.h>
/**
 * 32位寄存器结构体
 */
typedef union {
    unsigned int value;      // 整体32位访问
    struct {
        unsigned int low : 16;  // 低16位
        unsigned int high : 16; // 高16位
    } half;
    unsigned char bytes[4];  // 按字节访问
} Register32;

/**
 * 链表节点结构体
 */
typedef struct _node {
    int data;
    struct _node *next;
} Node;

/**
 * 创建新节点
 */
Node *create_node(int data) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void free_node(Node *node) {
    if (node != NULL) {
        free(node);
    }
}

Node *insert_node(Node *head, int data) {
    Node *new_node = create_node(data);
    if (head == NULL) {
        return new_node;
    }
    Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
    return head;
}
Node *delete_node(Node *head, int data) {
    if (head == NULL) {
        return NULL;
    }
    if (head->data == data) {
        Node *temp = head->next;
        free_node(head);
        return temp;
    }
    Node *current = head;
    while (current->next != NULL && current->next->data != data) {
        current = current->next;
    }
    if (current->next != NULL) {
        Node *temp = current->next;
        current->next = temp->next;
        free_node(temp);
    }
    return head;
}
Node *find_node(Node *head, int data) {
    Node *current = head;
    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
Node *free_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free_node(temp);
    }
    return NULL;
}
Node *print_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
    return head;
}
Node *reverse_list(Node *head) {
    Node *prev = NULL;
    Node *current = head;
    Node *next = NULL;
    while (current != NULL) {
        next = current->next; // 保存下一个节点
        current->next = prev; // 反转当前节点的指针
        prev = current;       // 移动 prev 和 current
        current = next;
    }
    return prev; // 新的头节点
}

int main() {
    Register32 reg;
    reg.value = 0x12345678;
    printf("size of Register32: %zu\n", sizeof(Register32));
    printf("reg.value = 0x%08X\n", reg.value);
    printf("reg.half.low = 0x%04X\n", reg.half.low);
    printf("reg.half.high = 0x%04X\n", reg.half.high);
    printf("reg.bytes[0] = 0x%02X\n", reg.bytes[0]);
    printf("reg.bytes[1] = 0x%02X\n", reg.bytes[1]);
    printf("reg.bytes[2] = 0x%02X\n", reg.bytes[2]);
    printf("reg.bytes[3] = 0x%02X\n", reg.bytes[3]);
    return 0;
}